#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL.h>    /*lib externe utilisée comme 'container' pour OpenGL*/
#include <ctype.h>  /*toupper*/
#include <string.h> /*strrchr*/

#define COLOUR 0xFF, 0xF0, 0x28 /*couleur entre le vert et le brun*/

/*
    - graine est la chaine qui contient le L-System
    - tab_regles est un tableau de règles de type <LETTRE>=<CHAINE DE CARACTERES>
    - angle est la valeur de l'angle (EN DEGRES) qui va être utilisé lors du symbole '+' ou '-'
    - longueur est la longueur (en pixels) de chaque trait
    - iteration est l'itération actuelle. Lorsque'elle vaut 1, on arrête la récursion et on dessine
*/
void parse_L_System_GL(const char *graine, const char *tab_regles[], double angle, int longueur, int iteration);

const char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void parse_L_System_GL(const char *graine, const char *tab_regles[], double angle, int longueur, int iteration)
{
    int i, j;
    for(i = 0; graine[i] != '\0'; ++i)
    {
        if(strrchr("+-", graine[i]))
            glRotated((graine[i] == '+' ? 1 : -1)*angle, 0, 0, 1);
        else if(graine[i] == '[')
            glPushMatrix();
        else if(graine[i] == ']')
            glPopMatrix();
        else if(strrchr(alphabet, toupper(graine[i])))
        {
            /*si on est à la fin de la récursion, alors*/
            if(iteration == 1)
            {
                /*on dessine la ligne*/
                glBegin(GL_LINES);
                    glColor3ub(COLOUR);
                    glVertex2i(0, 0);
                    glVertex2i(longueur, 0);
                glEnd();
                /*/!\ on n'oublie pas de déplacer le repère GL sinon tous les segments auront la même origine*/
                glTranslated(longueur, 0, 0);
            }
            /*sinon, on continue la récursion*/
            else
            {
                /*de toutes les règles qui sont passées en paramètre, on cherche celle qui commence par <graine[i]>=...*/
                for(j = 0; tab_regles[j] != NULL; ++j)
                {
                    if(tab_regles[j][0] == graine[i] && tab_regles[j][1] == '=')
                    {
                        /*récursion*/
                        parse_L_System_GL(&tab_regles[j][2], tab_regles, angle, longueur, iteration-1);
                        /*break sert ici à éviter les calculs inutiles :
                            lorsque l'on a trouvé la bonne règle, pas besoin de vérifier les suivantes*/
                        break;
                    }
                }
            }
        }
    }
}

/*choix lié à ma résolution, à changer sans problème*/
#define LARGEUR 1100
#define HAUTEUR 900

int main(int ac, char **av)
{
    const char *regle_dragon[] = {"X=X+YF+", "Y=-FX-Y", "F=F", NULL};
    const char *regle_plante[] = {"F=F[+F]F[-F]F", NULL};
    SDL_Event e;
    /*<Initialisation du container>*/
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        return EXIT_FAILURE;
    atexit(SDL_Quit);
    SDL_putenv("SDL_VIDEO_CENTERED=center");
    SDL_WM_SetCaption("L-Systèmes avec OpenGL : plante et dragon", NULL);
    SDL_SetVideoMode(LARGEUR, HAUTEUR, 32, SDL_OPENGL);
    /*</Initialisation>*/

    /*<Initialisation du modèle 2D par Pixels>*/
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, LARGEUR, 0, HAUTEUR);
    /*</Initialisation>*/

    /*<Initialisation du module de manipulation du repère>*/
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /*</Initialisation>*/


    glTranslated(120, 10, 0);
    glRotated(90, 0, 0, 1);

    /*exemple de fractale de type plante*/
    parse_L_System_GL("F", regle_plante, 25.7, 1, 7);

    glLoadIdentity();
    glTranslated(480, 500, 0);
    /*exemple de fractale de type dragon*/
    parse_L_System_GL("FX", regle_dragon, 90, 1, 17);

    /*Affichage*/
    glFlush();
    /*Swap du double buffering propre à OpenGL*/
    SDL_GL_SwapBuffers();

    /*Attente d'un évenement de type QUIT (croix rouge) ou APPUI SUR TOUCHE*/
    do
        SDL_WaitEvent(&e);
    while(e.type != SDL_QUIT && e.type != SDL_KEYDOWN);

    return 0;
    (void)ac;
    (void)av;
}
