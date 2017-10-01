#include <iostream>
#include <map>
#include <GLUT/glut.h>
using namespace std;

map<pair<int,int>,bool> vis;

class node{
public:
    node *nw,*ne,*se,*sw;
    int population;
    node(){
        nw=NULL;
        ne=NULL;
        se=NULL;
        sw=NULL;
        population=0;
    }
    ~node(){
        if(nw!=NULL){
            delete nw;
            delete ne;
            delete sw;
            delete se;
        }
    }
};

void box_draw(int i,int j,bool state_ij);
void display();
void my_init();
void my_keyboard(unsigned char key,int x,int y);
void my_mouse(int btn,int mouse_state,int x,int y);
void state_reset(int n,bool state[],int i,int j);
void insert_tree(node *n,int x,int y,int y1,int y2,int x1,int x2);
void findneighbour(int y,int x,bool* state,bool* state_new,node *newroot);
void nextGeneration();
void findgen(node *n,int x1,int x2,int y1,int y2,bool *state,bool *state_new,node* newroot);


int box_size;
bool *state;
int n;
int pixel_height;
int pixel_width;

node *root=new node();

int main (int argc,char *argv[])
{
    cout<<"Please enter the size of the board, in boxes (8 or 16 is a good value).\n";
    cin >>n;
    cout <<"\n";
    glutInit (&argc,argv);
    state=new bool[n*n];
    memset(state,0,n*n);
    my_init();
    return 0;
}

void insert_tree(node *n,int x,int y,int y1,int y2,int x1,int x2){
    //cout<<"insert tree called";
    //cout<<endl<<"x; "<<x<<" y:"<<y<<" x1: "<<x1<<" ,x2: "<<x2<<" ,y1: "<<y1<<" ,y2: "<<y2<<endl;
    if(x==x1 && y==y1 && x2-x1==1 && y2-y1==1){
        if(n->population==0){
            //cout<<"population of node set to 1"<<endl;
            n->population=1;
        }
        else{
            //cout<<"population of node set to 0"<<endl;
            n->population=0;
        }
        return;
        
    }
    else if(x2-x1==1 && y2-y1==1){
        //cout<<"already leaf node"<<endl;
        return;
    }
    int x3=(x1+x2)/2;
    int y3=(y1+y2)/2;
    if(n->nw==NULL){
        //cout<<"created x2 "<<x2<<endl;
        n->nw=new node();
        n->ne=new node();
        n->se=new node();
        n->sw=new node();
    }
    if(x>=x1 && x<x3 && y>=y1 && y<y3){
        insert_tree(n->nw,x,y,y1,y3,x1,x3);
    }
    else if(x>=x1 && x<x3 && y>=y3 && y<y2){
        insert_tree(n->sw,x,y,y3,y2,x1,x3);
    }
    else if(x>=x3 && x<x2 && y>=y1 && y<y3){
        insert_tree(n->ne,x,y,y1,y3,x3,x2);
    }
    else{
        insert_tree(n->se,x,y,y3,y2,x3,x2);
    }
    n->population=n->ne->population+n->nw->population+n->se->population+n->sw->population;
}

void findneighbour(int y,int x,bool* state,bool* state_new,node *newroot){
    //cout<<"findneighbour called"<<endl;
    if(x<0||x>=n||y<0||y>=n){
        //cout<<"out fo range"<<endl;
        return;
    }
    if(vis[make_pair(x,y)]==true){
        //cout<<"already visited node"<<endl;
        return;
    }
    int neighbors,i=x,j=y;
    neighbors = 0;
    if(i>0 && j>0)
    {
        neighbors=neighbors + state[(i-1)+(j-1)*n];
    }
    if(i>0)
    {
        neighbors += state[(i-1)+(j)*n];
    }
    if(i>0 && j <n-1)
    {
        neighbors += state[(i-1)+(j+1)*n];
    }
    if(j>0)
    {
        neighbors += state[(i)+(j-1)*n];
    }
    if(j<n-1)
    {
        neighbors += state[(i)+(j+1)*n];
    }
    if(i<n-1 && j>0)
    {
        neighbors += state[(i+1)+(j-1)*n];
    }
    if(i<n-1)
    {
        neighbors += state[(i+1)+(j)*n];
    }
    if(i<n-1 && j<n-1)
    {
        neighbors += state[(i+1)+(j+1)*n];
    }
    if(neighbors==3)
    {
        state_new[i+j*n] = true;
        insert_tree(newroot,j,i,0,n,0,n);
    }
    else if ( neighbors == 2 )
    {
        state_new[i+j*n] = state[i+j*n];
        if(state[i+j*n]){
            insert_tree(newroot,j,i,0,n,0,n);
        }
    }
    else
    {
        state_new[i+j*n] = false;
    }
    if(state[i+j*n] != state_new[i+j*n]){
        //cout<<"State Changed\n";
        //cout<<i<<" "<<j<<" "<<state_new[i+j*n]<<endl;
        box_draw(i,j,state_new[i+j*n]);
        glutPostRedisplay();
    }
    vis[make_pair(x,y)]=true;
}
void findgen(node *n,int x1,int x2,int y1,int y2,bool *state,bool *state_new,node* newroot){
    
    //cout<<"findgen called"<<endl;
    //cout<<" x1: "<<x1<<" ,x2: "<<x2<<" ,y1: "<<y1<<" ,y2: "<<y2<<endl;
    //cout<<"Population : "<<n->population<<endl;
    
    if(n->population==0){
        //cout<<"population 0,returned \n";
        return;
    }
    else if( x2-x1==1 || y2-y1==1 )
    {
        findneighbour(x1-1,y1-1,state,state_new,newroot);
        findneighbour(x1,y1-1,state,state_new,newroot);
        findneighbour(x1+1,y1-1,state,state_new,newroot);
        findneighbour(x1-1,y1,state,state_new,newroot);
        findneighbour(x1,y1,state,state_new,newroot);
        findneighbour(x1+1,y1,state,state_new,newroot);
        findneighbour(x1-1,y1+1,state,state_new,newroot);
        findneighbour(x1,y1+1,state,state_new,newroot);
        findneighbour(x1+1,y1+1,state,state_new,newroot);
        return;
    }
    else{
        findgen(n->nw,x1,(x1+x2)/2,y1,(y1+y2)/2,state,state_new,newroot);
        findgen(n->ne,(x1+x2)/2,x2,y1,(y1+y2)/2,state,state_new,newroot);
        findgen(n->sw,x1,(x1+x2)/2,(y1+y2)/2,y2,state,state_new,newroot);
        findgen(n->se,(x1+x2)/2,x2,(y1+y2)/2,y2,state,state_new,newroot);
    }
}
void nextGeneration(){
    //cout<<"new gen called "<<endl;
    vis.clear();
    bool *state_new;
    state_new = new bool[n*n];
    memset(state_new,0,n*n);
    node *newroot=new node();
    findgen(root,0,n,0,n,state,state_new,newroot);
    bool *tempstate=state;
    state=state_new;
    node *temp=root;
    
    delete temp;
    delete tempstate;
    
    root=newroot;
}


void box_draw(int i,int j,bool stateij)
{
    //glFlush();
    //cout<<"Box draw called\n";
    float p[2];
    GLfloat a;
    GLfloat b;
    GLfloat c;
    GLfloat grey[3] = {0.8,0.8,0.8 };
    GLfloat yellow[3] = {1.0,1.0,0.0};
    if(stateij){
        glColor3fv ( yellow );
    }
    else{
        glColor3fv( grey );
    }
    c=box_size;
    a=j*c;
    b=(n-1-i)*c;
    glBegin (GL_POLYGON);
    p[0]=a+2;
    p[1]=b+2;
    glVertex2fv (p);
    p[0]=a+c-2;
    p[1]=b+2;
    glVertex2fv(p);
    p[0]=a+c-2;
    p[1]=b+c-2;
    glVertex2fv(p);
    p[0]=a+2;
    p[1]=b+c-2;
    glVertex2fv(p);
    glEnd();
    return;
}


void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    for (int j= 0;j<n;j++){
        for (int i=0;i<n;i++){
            box_draw(i,j,state[i+j*n]);
        }
    }
    glutSwapBuffers();
    return;
}


void my_init()
{
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB );
    box_size=(500/n);
    pixel_width=500;
    pixel_height=500;
    glutInitWindowSize(pixel_width,pixel_height);
    glutCreateWindow("Game of Life");
    glClearColor(0.5,0.5,1.0,0.0);
    gluOrtho2D(0.0,(double)pixel_width,0.0,(double)pixel_height);
    glutDisplayFunc(display);
    glutKeyboardFunc(my_keyboard);
    glutMouseFunc(my_mouse);
    glutMainLoop();
    return;
}

void my_keyboard(unsigned char key,int x,int y)
{
    nextGeneration();
    return;
}

void my_mouse(int btn,int mouse_state,int x,int y)
{
    int i;
    int j;
    i=y/box_size;
    j=x/box_size;
    if((btn == GLUT_LEFT_BUTTON || btn == GLUT_MIDDLE_BUTTON ||  btn == GLUT_RIGHT_BUTTON )&& mouse_state == GLUT_DOWN )
    {
        state_reset (n,state,i,j);
        glutPostRedisplay();
        box_draw(i,j,state);
    }
    return;
}

void state_reset(int n,bool state[],int i,int j)
{
    if(i<0 || n<= i)
    {
        cout<< "STATE_RESET error!\n";
        cout<< "Illegal row number\n";
        exit(1);
    }
    
    if(j<0 || n <= j)
    {
        cout<<"\n";
        cout<<"STATE_RESET error!\n";
        cout<<"Illegal column number\n";
        exit(1);
    }
    state[i+j*n] = !state[i+j*n];
    insert_tree(root, j, i, 0, n, 0, n);
    return;
}
