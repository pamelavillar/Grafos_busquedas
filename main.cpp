#include <iostream>
#include "grafo.cpp"
#include <GLFW/glfw3.h>
#include <cmath> 

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Node* startNode = nullptr;
Node* endNode = nullptr;
bool selectingStart = true;
vector<bool> visited;
vector<int> predecesor;
deque<Node*> path;
Grafo grafo(10,10,6);

void drawNode(Node* n, float c1, float c2, float c3,float r=0.5f) {
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(c1,c2,c3); // azul
    glVertex2f(n->x, n->y );
    for(int i=0; i<=20; i++) {
        float angle = i * 2.0f * M_PI / 20;
        glVertex2f(n->x + cos(angle)*r, n->y + sin(angle)*r);
    }
    glEnd();
}


void drawEdge(Node* a, Node* b) {
    glBegin(GL_LINES);
    glColor3f(0.8f, 0.8f, 0.8f); // gris
    glVertex2f(a->x, a->y);
    glVertex2f(b->x, b->y);
    glEnd();
}

void drawPath(const deque<Node*>& path) {
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    glColor3f(1.0f, 1.0f, 0.0f); // amarillo
    for(Node* n : path) {
        glVertex2f(n->x, n->y);
    }
    glEnd();
    glLineWidth(1.0f);
}

void drawVisited(int at) {
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f); // rojo
    Node* a = grafo.nodos[at];
    Node* b = grafo.nodos[predecesor[at]];
    glVertex2f(a->x, a->y);
    glVertex2f(b->x, b->y);
    glEnd();
    glLineWidth(1.0f);
}

void drawVisitedNodes(const vector<bool>& visited) {
    glPointSize(10.0f);
    glBegin(GL_POINTS);
    glColor3f(1.0f, 0.0f, 0.0f); // rojo
    for(int at = 0; at<visited.size(); at++) {
        if(visited[at]){
            glVertex2f(grafo.nodos[at]->x, grafo.nodos[at]->y);//coloco un circulo en los nodos
            if (predecesor[at] != -1) { //pinto las aristas
                drawVisited(at);
            }
        }
    }
    glEnd();
    glPointSize(1.0f);
}


Node* pickNode(Grafo& grafo, double xpos, double ypos) {
    const float radius = 0.5f;
    for(Node* n : grafo.nodos) {
        float dx = xpos - n->x;
        float dy = ypos - n->y;
        if (dx*dx + dy*dy <= radius*radius) {
            return n;
        }
    }
    return nullptr;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        float glX = xpos / width * (grafo.col * grafo.tam_cuadrado + 2) - 1;
        float glY = (1 - ypos / height) * (grafo.filas * grafo.tam_cuadrado + 2) - 1;
        Node* picked = pickNode(grafo, glX, glY);
        if (picked) {
            if (selectingStart) {
                startNode = picked;
                endNode = nullptr;
                path.clear();
                selectingStart = false;
            } else {
                endNode = picked;
                auto [p,v,pr] = grafo.busqueda_profundidad(startNode, endNode);// ACA HAY QUE CAMBIAR MANUALMENTE EL TIPO DE BUSQUEDA
                path = p;
                visited = v;
                predecesor = pr;
                selectingStart = true;
            }
        }
    }
}

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(800, 600, "Grafo BFS", NULL, NULL);
    glfwMakeContextCurrent(window);

    grafo.generarGrafo();
    grafo.listaAdyacencia();

    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // configuracion de pantalla

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, grafo.col * grafo.tam_cuadrado + 1,  
            -1, grafo.filas * grafo.tam_cuadrado + 1, 
            -1, 1);                                  
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        //aristas
        for(int i=0;i<grafo.nodos.size();i++){
            for(Node* v: grafo.adyacencia[i]){
                drawEdge(grafo.nodos[i], v);
            }
        }

        //nodos
        for(Node* n : grafo.nodos){
            if (n == startNode || n == endNode) {
                drawNode(n,1.0f, 1.0f, 0.0f);
            } else {
                drawNode(n,0.2627f, 0.2627f, 0.2627f);
            }
        }
        // dibujar caminos visitados
        drawVisitedNodes(visited);
        

        // dibujar camino final 
        if (!path.empty()) {
            drawPath(path);    
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
