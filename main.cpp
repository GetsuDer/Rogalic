#include "common.h"
#include "Image.h"
#include "Player.h"
#include <iostream>
#include <fstream>
#include <ctime>

#define GLFW_DLL
#include <GLFW/glfw3.h>

constexpr GLsizei WINDOW_WIDTH = 1024, WINDOW_HEIGHT = 1024;
bool firing = false;

struct InputState
{
  bool keys[1024]{}; //массив состояний кнопок - нажата/не нажата
  GLfloat lastX = 400, lastY = 300; //исходное положение мыши
  bool firstMouse = true;
  bool captureMouse         = true;  // Мышка захвачена нашим приложением или нет?
  bool capturedMouseJustNow = false;
} static Input;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	switch (key) {
        case GLFW_KEY_ESCAPE:
	    	if (action == GLFW_PRESS)
		    	glfwSetWindowShouldClose(window, GL_TRUE);
		    break;
        case GLFW_KEY_1:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
        case GLFW_KEY_2:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            break;
	    default:
		    if (action == GLFW_PRESS)
                Input.keys[key] = true;
		    else if (action == GLFW_RELEASE) {
                Input.keys[key] = false;
                if (key == GLFW_KEY_Q) {
                    firing = true;
                }
            }
	}
}

void processPlayerMovement(Player &player, Maze **maze)
{
  if (firing) {
      firing = false;
      player.ProcessInput(MovementDir::FIRE, maze, Input.lastX, Input.lastY);
  }
  if (Input.keys[GLFW_KEY_W])
    player.ProcessInput(MovementDir::UP, maze);
  else if (Input.keys[GLFW_KEY_S])
    player.ProcessInput(MovementDir::DOWN, maze);
  if (Input.keys[GLFW_KEY_A])
    player.ProcessInput(MovementDir::LEFT, maze);
  else if (Input.keys[GLFW_KEY_D])
    player.ProcessInput(MovementDir::RIGHT, maze);
  else if (Input.keys[GLFW_KEY_E])
    player.ProcessInput(MovementDir::ACTION, maze);
  else player.ProcessInput(MovementDir::NONE, maze);
}

void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    Input.captureMouse = !Input.captureMouse;

  if (Input.captureMouse)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Input.capturedMouseJustNow = true;
  }
  else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (Input.firstMouse)
  {
    Input.lastX = float(xpos);
    Input.lastY = float(ypos);
    Input.firstMouse = false;
  }

  GLfloat xoffset = float(xpos) - Input.lastX;
  GLfloat yoffset = Input.lastY - float(ypos);

  Input.lastX = float(xpos);
  Input.lastY = float(ypos);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  // ...
}


int initGL()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  std::cout << "Controls: "<< std::endl;
  std::cout << "press right mouse button to capture/release mouse cursor  "<< std::endl;
  std::cout << "W, A, S, D - movement  "<< std::endl;
  std::cout << "press ESC to exit" << std::endl;

	return 0;
}

void
game_over(Image &screen) {
    Image you_died("resources/elems/you_died.png");
    int width = you_died.Width();
    int x_shift = screen.Width() / 3;
    int height = you_died.Height();
    int y_shift = screen.Height() / 2;
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            screen.PutPixel(i + x_shift, j + y_shift, you_died.GetPixel(i, height - j - 1));
        }   
    }
}

int
number_len(int n) {
    int res = 1;
    while (n > 9) {
        n /= 10;
        res++;
    }
    return res;
}

void
draw_digit(Image &screen, int pos, int digit, int x_shift, int y_shift) {
    std::string digit_name = std::to_string(digit);
    digit_name = "resources/elems/digits/" + digit_name + ".png";
    Image digit_img(digit_name);
    int width = digit_img.Width();
    int height = digit_img.Height();
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            screen.PutPixel(x_shift + width * pos + x, y_shift + y, digit_img.GetPixel(x, height - y - 1));
        }
    }
}

void
draw_info(Image &screen, Player &player, Maze *maze) {
    static Image keys_obtained("resources/elems/keys_obtained.png");
    int y_keys = tileSize * maze->size;
    static int width = keys_obtained.Width();
    static int height = keys_obtained.Height();
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            screen.PutPixel(x, y + y_keys, keys_obtained.GetPixel(x, height - y - 1));
        }
    }
    int n = player.keys_obtained;
    int n_len = number_len(n);
    static int n_max_len = 1;
    if (n_max_len < n_len) {
        n_max_len = n_len;
    }

    int digit_size = 64;
    for (int i = 0; i < n_max_len; i++) {
        for (int x = 0; x < digit_size; x++) {
           for (int y = 0; y < digit_size; y++) {
                screen.PutPixel(width + i * digit_size + x, y_keys + y, backgroundColor);
           }
        } 
    }
    while (n_len) {
        draw_digit(screen, n_len - 1, n % 10, width, y_keys);
        n /= 10;
        n_len--;
    } 
    
}

void
draw_win(Image &screen) {
    Image you_win("resources/elems/you_win.png");
    int width = you_win.Width();
    int x_shift = screen.Width() / 3;
    int height = you_win.Height();
    int y_shift = screen.Height() / 2;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            screen.PutPixel(x + x_shift, y + y_shift, you_win.GetPixel(x, height - y - 1));
        }
    }

}

std::string
room_type(int type) {
    switch (type) {
        case 0:
            return "roomA";
        case 1:
            return "roomB";
        case 2:
            return "roomC";
        default:
            return "roomD";
    }
    
}

Pixel make_darker(Pixel p, int n) {
    if (p.r < n ) {
        p.r = 0;
    } else {
        if (n < 0 && p.r - n > 255) p.r = 255;
        else p.r -= n;
    }
    if (p.g < n) {
        p.g = 0;
    } else {
        if (n < 0 && p.g - n > 255) p.g = 255;
        else p.g -= n;
    }
    if (p.b < n) {
        p.b = 0;
    } else {
        if (n < 0 && p.b - n > 255) p.b = 255;
        p.b -= n;
    }
    return p;
}

bool is_black(Pixel p) {
    return !p.r && !p.g && !p.b;
}
bool is_bigger(Pixel p1, Pixel p2) {
    if (p1.r > p2.r) return false;
    if (p1.g > p2.g) return false;
    if (p1.b > p2.b) return false;
    return true;
}

int main(int argc, char** argv)
{
	if(!glfwInit())
    return -1;

//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow*  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "task1 base project", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window); 

	glfwSetKeyCallback        (window, OnKeyboardPressed);  
	glfwSetCursorPosCallback  (window, OnMouseMove); 
  glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
	glfwSetScrollCallback     (window, OnMouseScroll);

	if(initGL() != 0) 
		return -1;
	
  //Reset any OpenGL errors which could be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();

    // here must read labirint
    std::ifstream labirint_in;
    labirint_in.open("resources/rooms/labirint");
    int rooms_number;
    std::string room_name;
    labirint_in >> rooms_number;
    std::vector <std::string> room_types(rooms_number);
    std::vector <Maze> rooms;
    std::string type;
    srand(time(0));
    for (int i = 0; i < rooms_number; i++) {
        room_name = "resources/rooms/" + std::to_string(i + 1);
        type = room_type(rand() % 4);
        rooms.push_back(Maze(room_name, type));
    }
    std::string start_room;
    labirint_in >> start_room;
    start_room = "resources/rooms/" + start_room; 
    Maze *current_room = NULL;

    for (int i = 0; i < rooms_number; i++) {
        if (rooms[i].path == start_room) {
            current_room = &(rooms[i]);
        }
        int doors_number;
        labirint_in >> doors_number;
        for (int j = 0; j < doors_number; j++) {
            int door_type;
            std::string other_room;
            labirint_in >> door_type >> other_room;
            other_room = "resources/rooms/" + other_room;
            for (int k = 0; k < rooms_number; k++) {
                if (rooms[k].path == other_room) {
                    rooms[i].others[door_type - 1] = &(rooms[k]);
                }
            }
        }
    }    
    labirint_in.close();
    Point starting_pos = current_room->Get_Player();
	Player player{starting_pos};
	Image screenBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 4);
    Image tmp_screenBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 4);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  GL_CHECK_ERRORS;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GL_CHECK_ERRORS;
  //game loop
	Maze *prev_room = current_room;
    while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
        glfwPollEvents();
        if (player.state == PlayerState::ALIVE) {
            processPlayerMovement(player, &current_room);
            if (prev_room != current_room) {
                prev_room->fires.clear();
                Pixel tmp;
                bool black;
                while (true) {
                    black = true;
                    for (int i = 0; i < WINDOW_WIDTH; i++) {
                        for (int j = 0; j < WINDOW_HEIGHT; j++) {
                            tmp = screenBuffer.GetPixel(i, j);
                            screenBuffer.PutPixel(i, j, make_darker(tmp, 5));
                            if (!is_black(tmp)) {
                                black = false;
                            }
                        }
                    }
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
                    glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
		            glfwSwapBuffers(window);

                    if (black) break;
                }
                prev_room = current_room;
                current_room->Draw_Lower(tmp_screenBuffer);
                player.Draw(tmp_screenBuffer);
                Pixel real;
                for (int add = 255; add > 0; add -= 10) {
                    for (int i = 0; i < WINDOW_WIDTH; i++) {
                        for (int j = 0; j < WINDOW_HEIGHT; j++) {
                            tmp = tmp_screenBuffer.GetPixel(i, j);
                            screenBuffer.PutPixel(i, j, make_darker(tmp, add));
                        }
                    }
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
                    glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
		            glfwSwapBuffers(window);
                }

            }
            current_room->processPlayer(player.placed());
            current_room->Draw_Lower(screenBuffer);
            player.Draw(screenBuffer);
            draw_info(screenBuffer, player, current_room);
            if (player.state == PlayerState::FELL) {
               // draw_fell(player, current_room);
		        Image *img = player.walk_animation.Draw();
                for (int i = 0; i < tileSize; i++) {
                    current_room->Draw_Lower(screenBuffer);
                    for (int x = 0; x < tileSize; x++) {
                        for (int y = i; y < tileSize; y++) {
                            screenBuffer.PutPixel(player.coords.x + x, 
                                    player.coords.y + y - i,
                                    blend(screenBuffer.GetPixel(player.coords.x + x,
                                            player.coords.y + y - i), img->GetPixel(x, tileSize - y - 1)));
                        }
                    }
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
                    glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
		            glfwSwapBuffers(window);
                }
                player.state = PlayerState::DEAD;
            }
            if (player.state == PlayerState::DEAD) {
                Pixel tmp;
                while (true) {
                    bool black = true;
                    for (int i = 0; i < WINDOW_WIDTH; i++) {
                        for (int j = 0; j < WINDOW_HEIGHT; j++) {
                            tmp = screenBuffer.GetPixel(i, j);
                            screenBuffer.PutPixel(i, j, make_darker(tmp, 10));
                            if (!is_black(tmp)) {
                                black = false;
                            }
                        }
                    }                         
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
                    glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
		            glfwSwapBuffers(window);
                    if (black) break;
                }
                game_over(screenBuffer);
            } else {
                if (player.state == PlayerState::WIN) {
                    Pixel tmp;
                    tmp.r = 255;
                    tmp.g = 255;
                    tmp.b = 255;
                    for (int i = 0; i * 2 < WINDOW_WIDTH; i++) {
                        for (int j = 0; j < WINDOW_HEIGHT; j++) {
                                screenBuffer.PutPixel(i, j, tmp);
                                screenBuffer.PutPixel(WINDOW_WIDTH - i - 1, j, tmp);
                        }
                        
                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
                        glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
		                glfwSwapBuffers(window);
                    }
                    draw_win(screenBuffer);        
                }
            }
        //    std::cout << Input.lastX << ' ' << Input.lastY << '\n';
            screenBuffer.PutPixel(Input.lastX, Input.lastY, backgroundColor);
        }
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
        glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}
