// main.cpp
#include <GL/glut.h>
#include "CellularAutomata.h"

// Declara uma instância global do autômato celular
CellularAutomata* ca;
int cellSize = 1; // Tamanho de cada célula

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    int width = ca->getWidth();
    int height = ca->getHeight();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int value = ca->getCell(x, y);

            // Define a cor com base na espécie
            if (value == 0) {
                glColor3f(1.0f, 1.0f, 1.0f); // Branco para a espécie 0
            } else if (value == 1) {
                glColor3f(1.0f, 0.0f, 0.0f); // Vermelho para a espécie 1
            } else if (value == 2) {
                glColor3f(0.0f, 0.0f, 1.0f); // Azul para a espécie 2
            } else if (value == 3) {
                glColor3f(0.0f, 1.0f, 0.0f); // Verde para a espécie 3
            } else {
                // Para mais espécies, calculamos a cor
                float hue = (value % 6) / 6.0f; // Usamos o módulo para garantir cores dentro do espectro
                glColor3f(hue, 1.0f - hue, 0.0f); // Gradiente entre vermelho e verde
            }
            
            glBegin(GL_QUADS);
            glVertex2f(x * cellSize, y * cellSize);
            glVertex2f((x + 1) * cellSize, y * cellSize);
            glVertex2f((x + 1) * cellSize, (y + 1) * cellSize);
            glVertex2f(x * cellSize, (y + 1) * cellSize);
            glEnd();
        }
    }

    glutSwapBuffers();
}

void update() {
    ca->step();
    
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    int width = 600;
    int height = 600;
    int speciesCount = 3; // Aumentamos o número de espécies para 3 para testar a cíclica
    NeighborhoodType neighborhood = MOORE;

    ca = new CellularAutomata(width, height, speciesCount, neighborhood);
    ca->generateRandomInitialState();
    //ca->display();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(width * cellSize, height * cellSize);
    glutCreateWindow("Cellular Automata");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width * cellSize, height * cellSize, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    glutDisplayFunc(display);
    glutIdleFunc(update);

    glutMainLoop();
    delete ca;

    return 0;
}
