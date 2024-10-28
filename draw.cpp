#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>

struct Point {
    float x, y;
    bool isEraser;
};

std::vector<Point> points;           
std::vector<Point> currentStroke;    
bool isDrawing = true;               
bool isMousePressed = false;         
double cursorX, cursorY;             
float eraseSize = 0.1f;              

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    
    int squareSize = (width < height) ? width : height;
    glViewport((width - squareSize) / 2, (height - squareSize) / 2, squareSize, squareSize);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isMousePressed = true;
            currentStroke.clear(); 

            
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            int width, height;
            glfwGetWindowSize(window, &width, &height);
            xpos = (xpos / width) * 2.0 - 1.0;  
            ypos = 1.0 - (ypos / height) * 2.0;

            
            if (xpos > -0.9f && xpos < -0.6f && ypos > 0.8f && ypos < 0.9f) {
                isDrawing = !isDrawing; 
            }
        } else if (action == GLFW_RELEASE) {
            isMousePressed = false;
            
            points.insert(points.end(), currentStroke.begin(), currentStroke.end());
            currentStroke.clear(); 
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    xpos = (xpos / width) * 2.0 - 1.0;  
    ypos = 1.0 - (ypos / height) * 2.0;

    cursorX = xpos;
    cursorY = ypos;

    if (isMousePressed) {
        if (isDrawing) {
            currentStroke.push_back({static_cast<float>(cursorX), static_cast<float>(cursorY), false});
        } else {
            
            for (auto it = points.begin(); it != points.end();) {
                float dx = it->x - cursorX;
                float dy = it->y - cursorY;
                if (sqrt(dx * dx + dy * dy) < eraseSize) {
                    it = points.erase(it); 
                } else {
                    ++it; 
                }
            }
        }
    }
}

void renderCircle(float x, float y, float radius) {
    int numSegments = 50;
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < numSegments; i++) {
        float theta = 2.0f * M_PI * float(i) / float(numSegments); 
        float dx = radius * cosf(theta); 
        float dy = radius * sinf(theta); 
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

void render() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT);

    
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    for (const auto& point : points) {
        if (point.isEraser) {
            glColor3f(1.0f, 1.0f, 1.0f); 
        } else {
            glColor3f(0.0f, 0.0f, 0.0f); 
        }
        glVertex2f(point.x, point.y);
    }
    glEnd();

    
    if (isMousePressed && isDrawing) {
        glBegin(GL_LINE_STRIP);
        glColor3f(0.0f, 0.0f, 0.0f); 
        for (const auto& point : currentStroke) {
            glVertex2f(point.x, point.y);
        }
        glEnd();
    }

    
    glColor3f(0.8f, 0.8f, 0.8f); 
    glBegin(GL_QUADS);
    glVertex2f(-0.9f, 0.9f);
    glVertex2f(-0.6f, 0.9f);
    glVertex2f(-0.6f, 0.8f);
    glVertex2f(-0.9f, 0.8f);
    glEnd();

    
    glColor3f(0.0f, 0.0f, 0.0f); 
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-0.9f, 0.9f);
    glVertex2f(-0.6f, 0.9f);
    glVertex2f(-0.6f, 0.8f);
    glVertex2f(-0.9f, 0.8f);
    glEnd();

    
    if (isDrawing) {
        glColor3f(0.0f, 0.0f, 0.0f); 
        glBegin(GL_QUADS);
        glVertex2f(-0.88f, 0.88f);
        glVertex2f(-0.82f, 0.88f);
        glVertex2f(-0.82f, 0.83f);
        glVertex2f(-0.88f, 0.83f);
        glEnd();
    } else {
        glColor3f(1.0f, 1.0f, 1.0f); 
        glBegin(GL_QUADS);
        glVertex2f(-0.88f, 0.88f);
        glVertex2f(-0.82f, 0.88f);
        glVertex2f(-0.82f, 0.83f);
        glVertex2f(-0.88f, 0.83f);
        glEnd();
        glColor3f(0.0f, 0.0f, 0.0f); 
        glBegin(GL_LINE_LOOP);
        glVertex2f(-0.88f, 0.88f);
        glVertex2f(-0.82f, 0.88f);
        glVertex2f(-0.82f, 0.83f);
        glVertex2f(-0.88f, 0.83f);
        glEnd();
    }

    
    if (!isDrawing) {
        glColor3f(0.8f, 0.8f, 0.8f); 
        renderCircle(static_cast<float>(cursorX), static_cast<float>(cursorY), eraseSize); 
    }
}

int main() {
    if (!glfwInit()) {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 800, "Vector Draw", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    
    while (!glfwWindowShouldClose(window)) {
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
