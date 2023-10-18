/******************************************************************
 * This program illustrates the fundamental instructions for handling
 * mouse and keyboeard events as well as menu buttons.
 */
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <stack>
#include <math.h>
#include <iostream>
#include <string>

#include <GL/freeglut.h>
#include <opencv2/opencv.hpp>

#define and &&
#define or ||

#define    SIZEX   800
#define    SIZEY   800

#define    MY_QUIT -1
#define    MY_CLEAR -2
#define    MY_SAVE  -3
#define    MY_BLEND -4
#define    MY_LOAD  -5

#define    WHITE   1
#define    RED     2
#define    GREEN   3
#define    BLUE    4

#define    POINT   1
#define    LINE    2
#define    POLYGON 3
#define    CIRCLE  4
#define    CURVE   5
#define    TYPE   6
#define    GRID   7

#define INF 0x3f3f3f3f
#define MXN 5000000

typedef    int   menu_t;
menu_t     top_m, color_m, file_m, type_m;

int        height = 150, width = 150;
unsigned char  image[MXN][4];  /* Image data in main memory */

int        pos_x = -1, pos_y = -1;
float      myColor[3] = { 0.0,0.0,0.0 };
int        obj_type = -1;
int        first = 0;      /* flag of initial points for lines and curve,..*/
int        vertex[128][2]; /*coords of vertices */
int        side = 0;         /*num of sides of polygon */
float      pnt_size = 1.0;

std::string input_string = "";


bool       ctrl = false, shift = false;
int        type_x = 0, type_y = 0;
int        load_image_width = INF, load_image_height = INF;
bool       keystates[256] = { false };


struct image_info{
  int width, height;
  std::vector<std::vector<unsigned char> > image;
  image_info(int width, int height, std::vector<std::vector<unsigned char> > image) : width(width), height(height), image(image){}
  image_info(){}
};
std::vector<std::vector<unsigned char> > v_image;

std::stack<image_info> undo_stack;
std::stack<image_info> redo_stack;


void save_motion(){
  glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE,
    image);
  // std::cout << "save_motion" << std::endl;
  std::vector<std::vector<unsigned char> > tmp_image(width * height, std::vector<unsigned char>(4, 0));
  // std::cout << "length: " << width * height << " vector: " << tmp_image.size() << std::endl;
  for(int i = 0; i < width; i++){
    for(int j = 0; j < height; j++){
      // std::cout << "width: " << width << " heigth: " << height << " length: " << width * height << " vector: " << tmp_image.size() << std::endl;
      // std::cout << "i: " << i << " j: " << j << " " << i * width + j << std::endl;
      tmp_image[i * height + j][0] = image[i * height + j][0];
      tmp_image[i * height + j][1] = image[i * height + j][1];
      tmp_image[i * height + j][2] = image[i * height + j][2];
      tmp_image[i * height + j][3] = image[i * height + j][3];
      // std::cout << i << ", " << j << "{" << (int) image[i * width + j][0] << ", " << (int) image[i * width + j][1] << ", " << (int) image[i * width + j][2] << ", " << (int) image[i * width + j][3] << "}" << std::endl;
    }
  }
  undo_stack.push(image_info(width, height, tmp_image));
  // std::cout << "undo stack size: " << undo_stack.size() << std::endl;
}

void undo_func(){
  // std::cout << "undo_func" << std::endl;
  if(undo_stack.size() == 2){
    for(int i = 0; i < width; i++){
      for(int j = 0; j < height; j++){
        image[i * height + j][0] = 0;
        image[i * height + j][1] = 0;
        image[i * height + j][2] = 0;
        image[i * height + j][3] = 0;
        // std::cout << "{" << (int) image[i * width + j][0] << ", " << (int) image[i * width + j][1] << ", " << (int) image[i * width + j][2] << ", " << (int) image[i * width + j][3] << "}" << " ";
      }
      // std::cout << std::endl;
    }
    glRasterPos2i(0, 0);
    glutReshapeWindow(width, height);
    glDrawPixels(width, height,
      GL_RGBA, GL_UNSIGNED_BYTE,
      image);
    glFlush();
    redo_stack.push(undo_stack.top());
    undo_stack.pop();
    return;
  }
  redo_stack.push(undo_stack.top());
  undo_stack.pop();

  std::vector<std::vector<unsigned char> > tmp_image = undo_stack.top().image;
  height = undo_stack.top().height;
  width = undo_stack.top().width;
  // undo_stack.pop();
  for(int i = 0; i < width; i++){
    for(int j = 0; j < height; j++){
      image[i * height + j][0] = tmp_image[i * height + j][0];
      image[i * height + j][1] = tmp_image[i * height + j][1];
      image[i * height + j][2] = tmp_image[i * height + j][2];
      image[i * height + j][3] = tmp_image[i * height + j][3];
      // std::cout << "{" << (int) image[i * width + j][0] << ", " << (int) image[i * width + j][1] << ", " << (int) image[i * width + j][2] << ", " << (int) image[i * width + j][3] << "}" << " ";
    }
    // std::cout << std::endl;
  }
  glRasterPos2i(0, 0);
  glutReshapeWindow(width, height);
  glDrawPixels(width, height,
    GL_RGBA, GL_UNSIGNED_BYTE,
    image);
  glFlush();
  // std::cout << "undo stack size: " << undo_stack.size() << std::endl;
  // std::cout << "redo stack size: " << redo_stack.size() << std::endl;
}

void redo_func(){
  // std::cout << "redo_func" << std::endl;
  std::vector<std::vector<unsigned char> > tmp_image = redo_stack.top().image;
  undo_stack.push(image_info(width, height, tmp_image));
  width = redo_stack.top().width;
  height = redo_stack.top().height;
  redo_stack.pop();
  for(int i = 0; i < width; i++){
    for(int j = 0; j < height; j++){
      image[i * height + j][0] = tmp_image[i * height + j][0];
      image[i * height + j][1] = tmp_image[i * height + j][1];
      image[i * height + j][2] = tmp_image[i * height + j][2];
      image[i * height + j][3] = tmp_image[i * height + j][3];
      // std::cout << "{" << (int) image[i * width + j][0] << ", " << (int) image[i * width + j][1] << ", " << (int) image[i * width + j][2] << ", " << (int) image[i * width + j][3] << "}" << " ";
    }
    // std::cout << std::endl;
  }
  glRasterPos2i(0, 0);
  glutReshapeWindow(width, height);
  glDrawPixels(width, height,
    GL_RGBA, GL_UNSIGNED_BYTE,
    image);
  glFlush();
  // std::cout << "undo stack size: " << undo_stack.size() << std::endl;
  // std::cout << "redo stack size: " << redo_stack.size() << std::endl;
}

void draw_string(){
  // std::cout << type_x << " " << type_y << " " << input_string << std::endl;
  glRasterPos2i(type_x, height - type_y);
  for(int i = 0; i < input_string.length(); i++){
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, input_string[i]);
  }
}

/*------------------------------------------------------------
 * Callback function for display, redisplay, expose events
 * Just clear the window again
 */
void display_func(void){
  /* define window background color */
  // glClear(GL_COLOR_BUFFER_BIT);
  draw_string();
  glFlush();
  // std::cout << "display_func" << std::endl;
}

/*-------------------------------------------------------------
 * reshape callback function for window.
 */
void my_reshape(int new_w, int new_h){
  if(load_image_height != INF and load_image_height != INF and (height < load_image_height || width < load_image_width)){
    // std::cout << "height: " << height << " width: " << width << std::endl;
    height = load_image_height;
    width = load_image_width;
    glutReshapeWindow(load_image_width, load_image_height);
    return;
  }
  height = new_h;
  width = new_w;
  // std::cout << "width: " << width << " height: " << height << std::endl;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (double) width, 0.0, (double) height);
  glViewport(0, 0, width, height);
  glMatrixMode(GL_MODELVIEW);

  glutPostRedisplay();   /*---Trigger Display event for redisplay window*/
  glFlush();
}


/*---------------------------------------------------------
 * Procedure to draw a polygon
 */
void draw_polygon(){
  int  i;

  glBegin(GL_POLYGON);
  glPointSize(pnt_size);

  for(i = 0; i < side; i++)
    glVertex2f(vertex[i][0], height - vertex[i][1]);
  glEnd();
  glFinish();
  side = 0;    /* set side=0 for next polygon */
  save_motion();
}

void draw_grid(int col, int row){
  glBegin(GL_LINES);
  glPointSize(pnt_size);

  for(int i = 0; i <= col; i++){
    glVertex2f(pos_x + i * 10 * pnt_size, height - pos_y);
    glVertex2f(pos_x + i * 10 * pnt_size, height - (pos_y + row * 10 * pnt_size));
  }
  for(int i = 0; i <= row; i++){
    glVertex2f(pos_x, height - (pos_y + i * 10 * pnt_size));
    glVertex2f(pos_x + col * 10 * pnt_size, height - (pos_y + i * 10 * pnt_size));
  }
  glEnd();
  glFinish();
}


/*------------------------------------------------------------
 * Procedure to draw a circle
 */
void draw_circle(){
  static GLUquadricObj *mycircle = NULL;

  if(mycircle == NULL){
    mycircle = gluNewQuadric();
    gluQuadricDrawStyle(mycircle, GLU_FILL);
  }
  glPushMatrix();
  glTranslatef(pos_x, height - pos_y, 0.0);
  gluDisk(mycircle,
    0.0,           /* inner radius=0.0 */
    pnt_size,          /* outer radius=10.0 */
    16,            /* 16-side polygon */
    3);
  glPopMatrix();
  save_motion();
}


/*------------------------------------------------------------
 * Callback function handling mouse-press events
 */
void mouse_func(int button, int state, int x, int y){
  if(button == GLUT_LEFT_BUTTON){
    if(state == GLUT_UP){
      if(obj_type == CURVE){
        save_motion();
      }
    }
    else{
      if(obj_type == TYPE){
        if(type_x != x and type_y != y){
          input_string = "";
          save_motion();
        }
        type_x = x;
        type_y = y;
        return;
      }
    }
  }
  if(button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
    return;

  switch(obj_type){
    case POINT:
    glPointSize(pnt_size);     /*  Define point size */
    glBegin(GL_POINTS);     /*  Draw a point */
    glVertex2f(x, height - y);
    glEnd();
    save_motion();
    break;
    case LINE:
    if(first == 0){
      first = 1;
      pos_x = x; pos_y = y;
      glPointSize(pnt_size);
      glBegin(GL_POINTS);   /*  Draw the 1st point */
      glVertex3f(x, height - y, 0);
      glEnd();
    }
    else{
      first = 0;
      glLineWidth(pnt_size);     /* Define line width */
      glBegin(GL_LINES);    /* Draw the line */
      glVertex2f(pos_x, height - pos_y);
      glVertex2f(x, height - y);
      glEnd();
      save_motion();
    }
    break;
    case POLYGON:  /* Define vertices of poly */
    if(side == 0){
      vertex[side][0] = x; vertex[side][1] = y;
      side++;
    }
    else{
      if(fabs(vertex[side - 1][0] - x) + fabs(vertex[side - 1][1] - y) < 2)
        draw_polygon();
      else{
        glBegin(GL_LINES);
        glVertex2f(vertex[side - 1][0], height - vertex[side - 1][1]);
        glVertex2f(x, height - y);
        glEnd();
        vertex[side][0] = x;
        vertex[side][1] = y;
        side++;
        save_motion();
      }
    }
    break;
    case CIRCLE:
    pos_x = x; pos_y = y;
    draw_circle();
    break;
    case GRID:
    pos_x = x; pos_y = y;
    int col, row;
    std::cin >> col >> row;
    draw_grid(col, row);
    break;
    default:
    break;
  }
  glFinish();
}

/*-------------------------------------------------------------
 * motion callback function. The mouse is pressed and moved.
 */
void motion_func(int  x, int y){
  if(obj_type != CURVE) return;
  if(first == 0){
    first = 1;
    pos_x = x; pos_y = y;
  }
  else{
    glBegin(GL_LINES);
    glPointSize(pnt_size);
    glVertex3f(pos_x, height - pos_y, 0.0);
    glVertex3f(x, height - y, 0.0);
    glEnd();
    pos_x = x; pos_y = y;
  }
  glFinish();
}

/*--------------------------------------------------------
 * procedure to clear window
 */
void init_window(void){
  /*Do nothing else but clear window to black*/

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (double) width, 0.0, (double) height);
  glViewport(0, 0, width, height);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClearColor(0.0, 0.0, 1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glFlush();
}


/*------------------------------------------------------
 * Procedure to initialize data alighment and other stuff
 */
void init_func(){
  glReadBuffer(GL_FRONT);
  glDrawBuffer(GL_FRONT);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  std::vector<std::vector<unsigned char> > tmp_image(1, std::vector<unsigned char>(4, 0));
  undo_stack.push(image_info(width, height, tmp_image));
}


void debug_func(){
  // glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE,
  //   image);
  // for(int i = 0; i < width; i++){
  //   for(int j = 0; j < height; j++){
  //     printf("{%d, %d, %d, %d} ", image[i * width + j][0], image[i * width + j][1], image[i * width + j][2], image[i * width + j][3]);
  //   }
  //   printf("\n");
  // }
  std::cout << "width: " << width << " height: " << height << std::endl;
  std::cout << "image width:" << load_image_width << " image height:" << load_image_height << std::endl;
}

void rotate_image(unsigned char image[MXN][4], cv::Mat img){
  std::vector<std::vector<unsigned char> > tmp_image(width * height, std::vector<unsigned char>(4, 0)), tmp_image2(width * height, std::vector<unsigned char>(4, 0));

  for(int i = 0; i < img.cols; i++){
    for(int j = 0; j < img.rows; j++){
      tmp_image2[i * img.rows + j][0] = image[i * img.rows + j][0];
      tmp_image2[i * img.rows + j][1] = image[i * img.rows + j][1];
      tmp_image2[i * img.rows + j][2] = image[i * img.rows + j][2];
      tmp_image2[i * img.rows + j][3] = image[i * img.rows + j][3];
    }
  }
  for(int i = 0; i < img.rows; i++){
    for(int j = 0; j < img.cols; j++){
      image[i * img.cols + j][0] = tmp_image2[j * img.rows + (img.rows - i - 1)][0];
      image[i * img.cols + j][1] = tmp_image2[j * img.rows + (img.rows - i - 1)][1];
      image[i * img.cols + j][2] = tmp_image2[j * img.rows + (img.rows - i - 1)][2];
      image[i * img.cols + j][3] = tmp_image2[j * img.rows + (img.rows - i - 1)][3];
    }
  }
}

void output_image(unsigned char image[MXN][4], const char *filename){
  cv::Mat img(height, width, CV_8UC4);
  // std::cout << "save file" << std::endl;

  for(int i = 0; i < width; i++){
    for(int j = 0; j < height; j++){
      if(image[i + j * width][3] == 0){
        img.at<cv::Vec4b>(j, i)[0] = 0;
        img.at<cv::Vec4b>(j, i)[1] = 0;
        img.at<cv::Vec4b>(j, i)[2] = 0;
        img.at<cv::Vec4b>(j, i)[3] = 255;
        continue;
      }
      img.at<cv::Vec4b>(j, i)[0] = image[i + j * width][2];
      img.at<cv::Vec4b>(j, i)[1] = image[i + j * width][1];
      img.at<cv::Vec4b>(j, i)[2] = image[i + j * width][0];
      img.at<cv::Vec4b>(j, i)[3] = image[i + j * width][3];
    }
  }

  for(int i = 0; i < width; i++){
    for(int j = 0; j < height / 2; j++){
      std::swap(img.at<cv::Vec4b>(j, i), img.at<cv::Vec4b>(height - j - 1, i));
    }
  }
  cv::imwrite(filename, img);
  std::cout << "save file done" << std::endl;
  // std::cout << "image size:" << height << " " << width << std::endl;
}

void read_image(unsigned char image[MXN][4], const char *filename){
  cv::Mat img = cv::imread(filename, cv::IMREAD_UNCHANGED);
  // std::cout << "read file" << std::endl;
  width = img.rows;
  height = img.cols;
  std::vector<std::vector<unsigned char> > tmp_image(width * height, std::vector<unsigned char>(4, 0)), tmp_image2(width * height, std::vector<unsigned char>(4, 0));
  for(int i = 0; i < img.cols; i++){
    for(int j = 0; j < img.rows; j++){
      image[i * img.rows + j][0] = img.at<cv::Vec4b>(j, i)[2];
      image[i * img.rows + j][1] = img.at<cv::Vec4b>(j, i)[1];
      image[i * img.rows + j][2] = img.at<cv::Vec4b>(j, i)[0];
      image[i * img.rows + j][3] = img.at<cv::Vec4b>(j, i)[3];
    }
  }
  rotate_image(image, img);
  std::swap(width, height);
  load_image_height = height;
  load_image_width = width;
  // for(int i = 0; i < img.rows; i++){
  //   for(int j = 0; j < img.cols; i++){
  //     image[i * img.cols + j][0] = tmp_image2[j * img.rows + i][0];
  //     image[i * img.cols + j][1] = tmp_image2[j * img.rows + i][1];
  //     image[i * img.cols + j][2] = tmp_image2[j * img.rows + i][2];
  //     image[i * img.cols + j][3] = tmp_image2[j * img.rows + i][3];
  //   }
  // }
  // for(int i = 0; i < width / 2; i++){
  //   for(int j = 0; j < height; j++)
  //     std::swap(image[i * width + j], image[(width - i - 1) * width + j]);
  // }
  for(int i = 0; i < width; i++){
    for(int j = 0; j < height; j++){
      tmp_image[i * height + j][0] = image[i * height + j][0];
      tmp_image[i * height + j][1] = image[i * height + j][1];
      tmp_image[i * height + j][2] = image[i * height + j][2];
      tmp_image[i * height + j][3] = image[i * height + j][3];
    }
  }
  undo_stack.push(image_info(width, height, tmp_image));
  std::cout << "read file done" << std::endl;
}

/*-----------------------------------------------------------------
 * Callback function for color menu
 */
void  color_func(int value){
  switch(value){
    case WHITE:
    myColor[0] = myColor[1] = myColor[2] = 1.0;
    break;

    case RED:
    myColor[0] = 1.0;
    myColor[1] = myColor[2] = 0.0;
    break;

    case GREEN:
    myColor[0] = myColor[2] = 0.0;
    myColor[1] = 1.0;
    break;

    case BLUE:
    myColor[0] = myColor[1] = 0.0;
    myColor[2] = 1.0;
    break;

    default:
    break;
  }
  glColor3f(myColor[0], myColor[1], myColor[2]);
}

/*------------------------------------------------------------
 * Callback function for top menu.
 */
void file_func(int value){
  int i, j;
  if(value == MY_QUIT) exit(0);
  else if(value == MY_CLEAR) init_window();
  else if(value == MY_SAVE){ /* Save current window */
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE,
      image);
    output_image(image, "image.png");
    /*
    for(i = 0; i < width; i++)   // Assign 0 opacity to black pixels
    for(j = 0; j < height; j++)
        if(image[i * width + j][0] == 0 &&
          image[i * width + j][1] == 0 &&
          image[i * width + j][2] == 0) image[i * width + j][3] = 0;
        else image[i * width + j][3] = 127; // Other pixels have A=127
    */
  }
  else if(value == MY_LOAD){ /* Restore the saved image */
    read_image(image, "image.png");
    glutReshapeWindow(width, height);
    glRasterPos2i(0, 0);
    glDrawPixels(width, height,
      GL_RGBA, GL_UNSIGNED_BYTE,
      image);
    // save_motion();
  }
  else if(value == MY_BLEND){ /* Blending current image with the saved image */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glRasterPos2i(0, 0);
    glDrawPixels(width, height,
      GL_RGBA, GL_UNSIGNED_BYTE,
      image);
    glDisable(GL_BLEND);
  }
  glFlush();
}

void size_func(int value){
  // std::cout << "call size_func " << value << std::endl;
  if(value == 1)
    if(pnt_size < 10.0) pnt_size += 1.0;

    else
      if(pnt_size > 1.0) pnt_size = pnt_size - 1.0;

}

/*---------------------------------------------------------------
 * Callback function for top menu. Do nothing.
 */
void top_menu_func(int value){}

/*-------------------------------------------------------------
 * Callback Func for type_m, define drawing object
 */
void draw_type(int value){
  obj_type = value;
  if(value == LINE || value == CURVE)
    first = 0;
  else if(value == POLYGON) side = 0;
}

/*--------------------------------------------------------------
 * Callback function for keyboard event.
 * key = the key pressed,
 * (x,y)= position in the window, where the key is pressed.
 */
void keyboardDown(unsigned char key, int x, int y){
  if(obj_type == TYPE and !ctrl){
    switch(key){
      //Enter
      case 13:
      input_string = "";
      save_motion();
      break;
      // Backspace
      case 8:
      if(input_string.length() > 0){
        // std::cout << "Back" << std::endl;
        input_string.pop_back();
      }
      break;
      default:
      input_string += key;
      break;
    }
    glutPostRedisplay();
    return;
  }
  // std::cout << "ctrl: " << ctrl << "shift: " << shift << "key: " << (int) key << std::endl;
  if(ctrl){
    switch(key){
      //17
      case 17: /* 27 = ESCAPE */
      exit(0);

      //3
      case 3: /* Clear window */
      init_window();
      break;

      //20
      case 20: /* Test whether overlay is available */
      debug_func();
      break;

      //19
      case 19: /* Save current window */
      file_func(MY_SAVE);
      break;

      //12
      case 12: /* Restore the saved image */
      file_func(MY_LOAD);
      break;

      // 26
      case 26:
      if(undo_stack.size() == 1) break;
      undo_func();
      break;

      //25
      case 25:
      if(redo_stack.empty()) break;
      redo_func();
      break;
    }
  }
  else if(shift){
    switch(key){
      case 43:
      size_func(1);
      break;
      case 95:
      size_func(2);
      break;
    }
  }
}

void specialKeyboardDown(int key, int x, int y){
  //ctrl
  if(key == 114)
    ctrl = true;
  else if(key == 112)
    shift = true;
}

void specialKeyboardUp(int key, int x, int y){
  if(key == 114)
    ctrl = false;
  // shift
  else if(key == 112)
    shift = false;

}

void keyboardUp(unsigned char key, int x, int y){
  keystates[key] = false;
}

/*---------------------------------------------------------------
 * Main procedure sets up the window environment.
 */
int main(int argc, char **argv){
  int  size_menu;

  glutInit(&argc, argv);    /*---Make connection with server---*/

  glutInitWindowPosition(0, 0);  /*---Specify window position ---*/
  glutInitWindowSize(width, height); /*--Define window's height and width--*/

  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA); /*---set display mode---*/
  init_func();

  /* Create parent window */
  glutCreateWindow("Menu");
  glClear(GL_COLOR_BUFFER_BIT);
  glutIgnoreKeyRepeat(1);
  glutDisplayFunc(display_func); /* Associate display event callback func */
  glutReshapeFunc(my_reshape);  /* Associate reshape event callback func */

  glutKeyboardFunc(keyboardDown); /* Callback func for keyboard event */
  glutKeyboardUpFunc(keyboardUp);
  glutSpecialFunc(specialKeyboardDown); /* Callback func for keyboard event */
  glutSpecialUpFunc(specialKeyboardUp);
  glutMouseFunc(mouse_func);  /* Mouse Button Callback func */
  glutMotionFunc(motion_func);/* Mouse motion event callback func */

  color_m = glutCreateMenu(color_func); /* Create color-menu */
  glutAddMenuEntry("white", WHITE);
  glutAddMenuEntry("red", RED);
  glutAddMenuEntry("green", GREEN);
  glutAddMenuEntry("blue", BLUE);

  file_m = glutCreateMenu(file_func);   /* Create another menu, file-menu */
  glutAddMenuEntry("save", MY_SAVE);
  glutAddMenuEntry("load", MY_LOAD);
  glutAddMenuEntry("blend", MY_BLEND);
  glutAddMenuEntry("clear", MY_CLEAR);
  glutAddMenuEntry("quit", MY_QUIT);

  type_m = glutCreateMenu(draw_type);   /* Create draw-type menu */
  glutAddMenuEntry("Point", POINT);
  glutAddMenuEntry("Line", LINE);
  glutAddMenuEntry("Poly", POLYGON);
  glutAddMenuEntry("Curve", CURVE);
  glutAddMenuEntry("Circle", CIRCLE);
  glutAddMenuEntry("Type", TYPE);
  glutAddMenuEntry("Grid", GRID);

  size_menu = glutCreateMenu(size_func);
  glutAddMenuEntry("Bigger", 1);
  glutAddMenuEntry("Smaller", 2);

  top_m = glutCreateMenu(top_menu_func);/* Create top menu */
  glutAddSubMenu("colors", color_m);    /* add color-menu as a sub-menu */
  glutAddSubMenu("type", type_m);
  glutAddSubMenu("Size", size_menu);
  glutAddSubMenu("file", file_m);       /* add file-menu as a sub-menu */
  glutAttachMenu(GLUT_RIGHT_BUTTON);    /* associate top-menu with right but*/

  /*---Test whether overlay support is available --*/
  if(glutLayerGet(GLUT_OVERLAY_POSSIBLE)){
    fprintf(stderr, "Overlay is available\n");
  }
  else{
    fprintf(stderr, "Overlay is NOT available, May encounter problems for menu\n");
  }
  /*---Enter the event loop ----*/
  glutMainLoop();
}
