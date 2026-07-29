#include "../../include/visualizer/AckermannStackEngine.hpp"
#include "../../include/visualizer/AudioSynthesizer.hpp"
#include "../../include/visualizer/QuantumTreeRenderer.hpp"
#include "../../include/visualizer/Landscape3DRenderer.hpp"
#include "../../include/visualizer/SpiralOrbitRenderer.hpp"
#include "../../include/visualizer/RaceEngine.hpp"
#include "../../include/visualizer/RaceArenaRenderer.hpp"
#include "../../include/visualizer/FontRenderer.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <vector>

using namespace Engine;

enum class RenderMode { QUANTUM_TREE = 1, LANDSCAPE_3D = 2, SPIRAL_ORBIT = 3, RACE_ARENA = 4 };

int main(int argc, char* argv[]) {
    // High-complexity default for ~5 minutes of continuous visual evolution: A(3, 3) = 61 (2,432 steps!)
    int currentM = 3;
    int currentN = 3;

    if (argc >= 2) {
        currentM = std::atoi(argv[1]);
    }
    if (argc >= 3) {
        currentN = std::atoi(argv[2]);
    }

    std::cout << "========================================================\n";
    std::cout << "  ThreadRace | Native C++ Ackermann Visualizer Suite\n";
    std::cout << "========================================================\n";
    std::cout << "  Loaded Input Parameters: A(" << currentM << ", " << currentN << ")\n";
    std::cout << "  Controls:\n";
    std::cout << "  [1] Quantum Tree  [2] 3D Landscape  [3] Spiral Orbit  [4] Thread Race Arena\n";
    std::cout << "  [SPACE/P] Play/Pause  [S] Single Step  [R] Reset/Restart\n";
    std::cout << "  [  /  ] Change m parameter (" << currentM << ")\n";
    std::cout << "  [ - / + ] Change n parameter (" << currentN << ")\n";
    std::cout << "  [I] Zoom In  [O] Zoom Out (Scalable 0.01x - 20x)\n";
    std::cout << "  [M] Toggle Audio Sound Synthesis\n";
    std::cout << "  [LEFT/RIGHT] Adjust Delay (ms per step)\n";
    std::cout << "  [H] Toggle HUD On-Screen Controls & Demystifier Trajectory\n";
    std::cout << "  [Mouse Drag/Wheel] Pan & Zoom Camera\n";
    std::cout << "  [ESC/Q] Quit\n";
    std::cout << "========================================================\n\n";

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    int screenWidth = 1280;
    int screenHeight = 720;

    SDL_Window* window = SDL_CreateWindow(
        "ThreadRace - Pure C++ Ackermann Visualizer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenWidth, screenHeight,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1); // Enable VSync

    AckermannStackEngine engine;
    RaceEngine raceEngine;
    AudioSynthesizer audio;

    QuantumTreeRenderer treeRenderer;
    Landscape3DRenderer landscapeRenderer;
    SpiralOrbitRenderer orbitRenderer;
    RaceArenaRenderer arenaRenderer;

    audio.init();
    FontRenderer::getInstance().init();

    // Subscribe audio and edge particle events to engine
    engine.setEventCallback([&](const EngineEvent& ev) {
        if (ev.type == EngineEventType::PUSH) {
            audio.playNote(ev.depth, false);
            const auto& nodes = engine.getNodes();
            auto it = nodes.find(ev.nodeId);
            if (it != nodes.end() && it->second.m == 0) {
                treeRenderer.spawnStarburst(it->second.x, it->second.y, 1.0f, 0.84f, 0.0f);
            }
        } else if (ev.type == EngineEventType::RESOLVE) {
            audio.playNote(ev.depth, true);
        } else if (ev.type == EngineEventType::COMPLETE) {
            audio.playComplete();
        }
    });

    engine.start(currentM, currentN);
    raceEngine.start(currentM, currentN, 4);

    RenderMode currentMode = RenderMode::RACE_ARENA; // Start in HEAD-TO-HEAD RACE ARENA MODE!
    bool isPlaying = true;
    bool showHUD = true; // Toggle HUD with 'H'
    Uint32 stepDelayMs = 150; // Initial slow pace of 150ms/step
    Uint32 lastStepTime = SDL_GetTicks();

    // Global Camera State across ALL visualizer modes!
    float camX = 0.0f, camY = 0.0f, camZoom = 1.0f;
    float targetCamX = 0.0f, targetCamY = 0.0f, targetCamZoom = 1.0f;

    bool isMouseDown = false;
    int lastMouseX = 0, lastMouseY = 0;
    bool running = true;

    while (running) {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                running = false;
            } else if (ev.type == SDL_WINDOWEVENT) {
                if (ev.window.event == SDL_WINDOWEVENT_RESIZED) {
                    screenWidth = ev.window.data1;
                    screenHeight = ev.window.data2;
                    glViewport(0, 0, screenWidth, screenHeight);
                }
            } else if (ev.type == SDL_MOUSEBUTTONDOWN) {
                if (ev.button.button == SDL_BUTTON_LEFT) {
                    isMouseDown = true;
                    lastMouseX = ev.button.x;
                    lastMouseY = ev.button.y;
                }
            } else if (ev.type == SDL_MOUSEBUTTONUP) {
                if (ev.button.button == SDL_BUTTON_LEFT) {
                    isMouseDown = false;
                }
            } else if (ev.type == SDL_MOUSEMOTION) {
                if (isMouseDown) {
                    int dx = ev.motion.x - lastMouseX;
                    int dy = ev.motion.y - lastMouseY;
                    targetCamX += dx / targetCamZoom;
                    targetCamY += dy / targetCamZoom;
                    lastMouseX = ev.motion.x;
                    lastMouseY = ev.motion.y;
                }
            } else if (ev.type == SDL_MOUSEWHEEL) {
                if (ev.wheel.y > 0) targetCamZoom = std::max(0.01f, std::min(20.0f, targetCamZoom * 1.15f));
                else if (ev.wheel.y < 0) targetCamZoom = std::max(0.01f, std::min(20.0f, targetCamZoom * 0.87f));
            } else if (ev.type == SDL_KEYDOWN) {
                switch (ev.key.keysym.sym) {
                    case SDLK_ESCAPE: case SDLK_q:
                        running = false;
                        break;
                    case SDLK_h:
                        showHUD = !showHUD;
                        std::cout << "[Visualizer] HUD " << (showHUD ? "SHOWN" : "HIDDEN") << std::endl;
                        break;
                    case SDLK_i: // Zoom In across all modes
                        targetCamZoom = std::max(0.01f, std::min(20.0f, targetCamZoom * 1.25f));
                        break;
                    case SDLK_o: // Zoom Out across all modes
                        targetCamZoom = std::max(0.01f, std::min(20.0f, targetCamZoom * 0.8f));
                        break;
                    case SDLK_SPACE: case SDLK_p:
                        isPlaying = !isPlaying;
                        std::cout << "[Visualizer] " << (isPlaying ? "PLAYING" : "PAUSED") << std::endl;
                        break;
                    case SDLK_s:
                        engine.step();
                        raceEngine.step();
                        treeRenderer.updateLayout(engine);
                        break;
                    case SDLK_r:
                        engine.start(currentM, currentN);
                        raceEngine.start(currentM, currentN, 4);
                        targetCamX = 0.0f; targetCamY = 0.0f; targetCamZoom = 1.0f;
                        isPlaying = true;
                        std::cout << "[Visualizer] RESET A(" << currentM << ", " << currentN << ")" << std::endl;
                        break;
                    case SDLK_LEFTBRACKET: // Decrement m
                        currentM = std::max(0, currentM - 1);
                        engine.start(currentM, currentN);
                        raceEngine.start(currentM, currentN, 4);
                        targetCamX = 0.0f; targetCamY = 0.0f; targetCamZoom = 1.0f;
                        isPlaying = true;
                        std::cout << "[Visualizer] Input set to A(" << currentM << ", " << currentN << ")" << std::endl;
                        break;
                    case SDLK_RIGHTBRACKET: // Increment m
                        currentM = std::min(4, currentM + 1);
                        engine.start(currentM, currentN);
                        raceEngine.start(currentM, currentN, 4);
                        targetCamX = 0.0f; targetCamY = 0.0f; targetCamZoom = 1.0f;
                        isPlaying = true;
                        std::cout << "[Visualizer] Input set to A(" << currentM << ", " << currentN << ")" << std::endl;
                        break;
                    case SDLK_MINUS: // Decrement n
                        currentN = std::max(0, currentN - 1);
                        engine.start(currentM, currentN);
                        raceEngine.start(currentM, currentN, 4);
                        targetCamX = 0.0f; targetCamY = 0.0f; targetCamZoom = 1.0f;
                        isPlaying = true;
                        std::cout << "[Visualizer] Input set to A(" << currentM << ", " << currentN << ")" << std::endl;
                        break;
                    case SDLK_EQUALS: case SDLK_PLUS: // Increment n
                        currentN = std::min(10, currentN + 1);
                        engine.start(currentM, currentN);
                        raceEngine.start(currentM, currentN, 4);
                        targetCamX = 0.0f; targetCamY = 0.0f; targetCamZoom = 1.0f;
                        isPlaying = true;
                        std::cout << "[Visualizer] Input set to A(" << currentM << ", " << currentN << ")" << std::endl;
                        break;
                    case SDLK_1:
                        currentMode = RenderMode::QUANTUM_TREE;
                        std::cout << "[Visualizer] Switched to Mode 1: Quantum Tree" << std::endl;
                        break;
                    case SDLK_2:
                        currentMode = RenderMode::LANDSCAPE_3D;
                        std::cout << "[Visualizer] Switched to Mode 2: 3D Landscape" << std::endl;
                        break;
                    case SDLK_3:
                        currentMode = RenderMode::SPIRAL_ORBIT;
                        std::cout << "[Visualizer] Switched to Mode 3: Spiral Orbit" << std::endl;
                        break;
                    case SDLK_4:
                        currentMode = RenderMode::RACE_ARENA;
                        std::cout << "[Visualizer] Switched to Mode 4: Head-to-Head Thread Race Arena" << std::endl;
                        break;
                    case SDLK_m:
                        audio.setEnabled(!audio.isEnabled());
                        std::cout << "[Visualizer] Sound " << (audio.isEnabled() ? "ON" : "OFF") << std::endl;
                        break;
                    case SDLK_LEFT: // Slower (increase delay)
                        stepDelayMs = std::min<Uint32>(1000, stepDelayMs + 25);
                        std::cout << "[Visualizer] Pacing Delay: " << stepDelayMs << " ms/step" << std::endl;
                        break;
                    case SDLK_RIGHT: // Faster (decrease delay)
                        stepDelayMs = (stepDelayMs <= 25) ? 5 : stepDelayMs - 25;
                        std::cout << "[Visualizer] Pacing Delay: " << stepDelayMs << " ms/step" << std::endl;
                        break;
                }
            }
        }

        // Smooth camera movement interpolation
        camX += (targetCamX - camX) * 0.1f;
        camY += (targetCamY - camY) * 0.1f;
        camZoom += (targetCamZoom - camZoom) * 0.1f;

        // Time-paced simulation step update
        Uint32 now = SDL_GetTicks();
        if (isPlaying && (now - lastStepTime >= stepDelayMs)) {
            engine.step();
            raceEngine.step();
            treeRenderer.updateLayout(engine);
            lastStepTime = now;
        }

        // Window Title Telemetry Update
        Metrics metrics = engine.getMetrics();
        std::string title = "ThreadRace C++ Ackermann Visualizer | A(" + std::to_string(metrics.m) + ", " + std::to_string(metrics.n) +
            ") | Steps: " + std::to_string(metrics.stepCount) + " | Depth: " + std::to_string(metrics.currentDepth) +
            " | Peak: " + std::to_string(metrics.maxDepth) + " | Pace: " + std::to_string(stepDelayMs) + "ms" +
            (metrics.isCompleted ? " [COMPLETED]" : " [RUNNING]");
        SDL_SetWindowTitle(window, title.c_str());

        // Clear Screen
        glClearColor(0.027f, 0.035f, 0.055f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render current visualizer mode!
        if (currentMode == RenderMode::QUANTUM_TREE) {
            glDisable(GL_DEPTH_TEST);
            treeRenderer.render(engine, screenWidth, screenHeight, camX, camY, camZoom);
        } else if (currentMode == RenderMode::LANDSCAPE_3D) {
            landscapeRenderer.render(engine, screenWidth, screenHeight, camX, camY, camZoom);
        } else if (currentMode == RenderMode::SPIRAL_ORBIT) {
            glDisable(GL_DEPTH_TEST);
            orbitRenderer.render(engine, screenWidth, screenHeight, camX, camY, camZoom);
        } else if (currentMode == RenderMode::RACE_ARENA) {
            glDisable(GL_DEPTH_TEST);
            arenaRenderer.render(raceEngine, screenWidth, screenHeight);
        }

        // Render On-Screen HUD overlay using crisp Game-Engine Texture Atlas Font
        glDisable(GL_DEPTH_TEST);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, screenWidth, screenHeight, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        if (showHUD && currentMode != RenderMode::RACE_ARENA) {
            // Top Left HUD Control Panel Card (modes 1, 2, 3)
            std::vector<std::string> topHudLines = {
                "EVALUATION TASK: A(" + std::to_string(metrics.m) + ", " + std::to_string(metrics.n) + ")" + (metrics.isCompleted ? " [DONE]" : " [RUNNING]"),
                "STEPS: " + std::to_string(metrics.stepCount) + "   STACK DEPTH: " + std::to_string(metrics.currentDepth) + "   PACE: " + std::to_string(stepDelayMs) + " MS",
                "CONTROLS: [SPACE] PLAY/PAUSE   [R] RESTART   [1/2/3/4] MODES",
                "          [I] ZOOM IN   [O] ZOOM OUT (0.01X - 20X)",
                "          [LEFT/RIGHT] SPEED   [M] SOUND     [H] HIDE HUD"
            };
            FontRenderer::getInstance().renderCardHUD(15.0f, 15.0f, 540.0f, 130.0f, topHudLines);

            // Top Right Live DSA Call Stack Execution Trajectory Card
            const auto& stack = engine.getStack();
            const auto& nodes = engine.getNodes();
            std::vector<std::string> stackTrajectoryLines;
            stackTrajectoryLines.push_back("LIVE RECURSION STACK TRAJECTORY:");
            
            int stackSize = static_cast<int>(stack.size());
            int showCount = std::min<int>(5, stackSize);
            for (int i = stackSize - 1; i >= stackSize - showCount; --i) {
                if (i < 0 || i >= stackSize) break;
                const auto& frame = stack[i];
                int depth = i + 1;
                auto it = nodes.find(frame.nodeId);
                if (it != nodes.end()) depth = it->second.depth;

                stackTrajectoryLines.push_back("  [DEPTH " + std::to_string(depth) + "] A(" + std::to_string(frame.m) + ", " + std::to_string(frame.n) + ")");
            }
            if (stackTrajectoryLines.size() == 1) {
                stackTrajectoryLines.push_back("  [STACK EMPTY - COMPLETED]");
            }
            float rightCardW = 340.0f;
            float rightCardX = static_cast<float>(screenWidth) - rightCardW - 15.0f;
            FontRenderer::getInstance().renderCardHUD(rightCardX, 15.0f, rightCardW, 130.0f, stackTrajectoryLines);

            // Bottom Description Banner Card explaining current mode
            std::vector<std::string> bottomDescLines;
            if (currentMode == RenderMode::QUANTUM_TREE) {
                bottomDescLines = {
                    "MODE 1: QUANTUM BRANCHING CALL TREE",
                    "Visualizes stack expansion. Cyan = Active calls, Gold = Base cases m=0, Green = Resolved values."
                };
            } else if (currentMode == RenderMode::LANDSCAPE_3D) {
                bottomDescLines = {
                    "MODE 2: 3D HYPER-LOGARITHMIC SURFACE LANDSCAPE",
                    "Maps A(m,n) growth onto Z = ln(1 + ln(1 + A(m,n))) heightmap. Terrain peaks pulse with live recursion."
                };
            } else if (currentMode == RenderMode::SPIRAL_ORBIT) {
                bottomDescLines = {
                    "MODE 3: GOLDEN RATIO PHASE-SPACE SPIRAL ORBIT",
                    "Projects step order & stack depth onto a logarithmic Golden Ratio phi = 1.618 galaxy orbit."
                };
            }

            float cardY = static_cast<float>(screenHeight) - 75.0f;
            float cardW = std::min(1100.0f, static_cast<float>(screenWidth) - 30.0f);
            FontRenderer::getInstance().renderCardHUD(15.0f, cardY, cardW, 60.0f, bottomDescLines);
        }

        SDL_GL_SwapWindow(window);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "\nVisualizer closed cleanly.\n";
    return 0;
}
