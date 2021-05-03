//
//		          Programming Assignment #1
//
//			        Victor Zordan
//
//
//
/***************************************************************************/

                                                   /* Include needed files */
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

#include <GL/glut.h>   // The GL Utility Toolkit (Glut) Header

#define WIDTH 500
#define HEIGHT 500

int x_last,y_last;
int stub = 1;
ifstream objFile;
string filename;
bool horizArray = false;
double xTrans = 0;
double yTrans = 0;
double zTrans = 0;
double horizTheta = 0;
double vertTheta = 0;
double scaleFactor = 1;
vector <double> Xvertices;
vector <double> Yvertices;
vector <double> Zvertices;
vector <double> currentXvert;
vector <double> currentYvert;
vector <double> currentZvert;
vector <int> firstVertices;
vector <int> secondVertices;
vector <int> thirdVertices;

/***************************************************************************/

void init_window()
                 /* Clear the image area, and set up the coordinate system */
{

        					       /* Clear the window */
        glClearColor(0.0,0.0,0.0,0.0);
	glShadeModel(GL_SMOOTH);
        glOrtho(0,WIDTH,0,HEIGHT,-1.0,1.0);
}

/***************************************************************************/

void write_pixel(int x, int y, double intensity)
                                         /* Turn on the pixel found at x,y */
{

        glColor3f (intensity, intensity, intensity);
        glBegin(GL_POINTS);
           glVertex3i( x, y, 0);
        glEnd();
}

//***************************************************************************/

/* Turn on the pixels in a line connecting the two points using DDA */
void write_line(int startX, int startY, int endX, int endY)
{
    double x_inc;            //Change in the x direction between two points
    double y_inc;            //Change in the y direction between two points
    double slope;            //The slope of the line
    double x;                //Will represent x values as we move along the line
    double y;                //Will represent y values as we move along the line

    slope = (double(endY) - double(startY)) / (double(endX) - double(startX));

    /* The way that I implemented DDA assumes that the first input point is to
    left of the second point. This swaps the order of the inputs if they are
    entered in a way that the first vertex is to the right of the second vertex*/
    if(endX < startX)
    {
      int tempX = startX;
      int tempY = startY;
      startX = endX;
      startY = endY;
      endX = tempX;
      endY = tempY;
    }

    x = startX;
    y = startY;

   if(slope >= 0)                //Slope is positive
   {
     if(slope < 1)               //Slope is positive and shallow
     {
       /*Since the slope is shallow the x values will need to always increase by
       one every time and the y values will need to increase according to slope.
       This is done to preserve the intensity of the line.*/
       x_inc = 1;
       y_inc = slope;

       //This loop actually draws the line
       for(int i = 0; i < abs(endX - startX); i++)
       {
         write_pixel(x, y, 1);      //Turn the pixel at x,y on
         x = x + x_inc;
         y = y + y_inc;          //Move y so that it is on the next position on the line
       }
     }
     else                        //Slope is positive and steep
     {
       /*Since the slope is steep the y values will need to always increase by
       one and the x values will need to increase according to slope.
       This is done to preserve the intensity of the line.*/
       x_inc = 1 / slope;
       y_inc = 1;

       //This loop actually draws the line
       for(int i = 0; i < abs(endY - startY); i++)
       {
         write_pixel(x, y, 1);    //Turn the pixel at x,y on
         x = x + x_inc;        //Move x so that it is on the next position on the line
         y = y + y_inc;        //Move y so that it is on the next position on the line
       }
     }
   }
   else if(slope < 0)            //Slope is negative
   {
     if(slope > -1)              //Slope is negative and shallow
     {
       x_inc = 1;
       y_inc = slope;
       for(int i = 0; i < abs(endX - startX); i++)
       {
         write_pixel(x, y, 1);
         x = x + x_inc;
         y = y + y_inc;
       }
     }
     else if(slope < -1)         //Slope is negative and steep
     {
       x_inc = 1 / slope;
       y_inc = 1;
       for(int i = 0; i < abs(endY - startY); i++)
       {
         write_pixel(x, y, 1);
         x = x - x_inc;
         y = y - y_inc;
       }
     }
   }
   /*In this case the line is vertical so no movement is required of x. All the
   adjustment is purely vertical*/
   else if(startX == endX)
   {
     y_inc = 1;
     for(int i = 0; i < abs(endY - startY); i++)
     {
       write_pixel(x, y, 1);
       y = y + y_inc;
     }
   }
}

/***************************************************************************/
//This function draws the line between the two points it is given
void draw_line(int vert1X, int vert1Y, int vert2X, int vert2Y)
{
    write_line(vert1X, vert1Y, vert2X, vert2Y);
}

/***************************************************************************/
/*This function draws a triangle between three points that are entered by the
user. The triangle is made buy calling the write_line function three times to
connect all the points*/
void draw_triangle(int crn1X, int crn1Y, int crn2X, int crn2Y, int crn3X, int crn3Y)
{
    write_line(crn1X, crn1Y, crn2X, crn2Y);
    write_line(crn2X, crn2Y, crn3X, crn3Y);
    write_line(crn3X, crn3Y, crn1X, crn1Y);
}

/***************************************************************************/
/*This function opens up the object file containing the model info as a command
line arguement*/
void ObtainFileFromUser()
{
  objFile.open(filename.c_str());
}

//***************************************************************************/
/*This function takes in a number that is in the form of a string and converts
it into a double. It used to convert numbers from the object file into numbers
the program can use.*/
double str2Num(string str)
{
  stringstream converter;
  double num;
  converter << str;
  converter >> num;
  return num;
}

//***************************************************************************/
/*This function splits a string into multiple substrings based on a passed in
delimiter. It used to parse strings that are taken from the object file*/
vector<string> splitUpLine(string lineOfText, char delimiter)
{
  vector<string> strArray;
  string subString;
  size_t pos = 0;

  /*While the lineOfText has a delimiter in it cut off from the start of the
  string to the delimiter and store that substring. Then delete the substring
  from the original string and repeat.*/
  while((pos = lineOfText.find(delimiter)) != string::npos)
  {
    subString = lineOfText.substr(0, pos);
    strArray.push_back(subString);
    lineOfText.erase(0, pos + sizeof(char));
  }
  return strArray;
}

//***************************************************************************/
/*This function reads from the object file that was passed in and grabs the
vertex and face values that are needed to make the 3D model later in the
program*/
void ParseFile()
{
  string currentLine;
  vector <string> faces;

  while(!objFile.eof())
  {
    //Grabs a line from the file and store it as a string
    getline(objFile, currentLine);

    /*This adds a deliimiter to the end of the line so that the line can be
    separated with splitUpLine()*/
    currentLine += " ";

    //if the current line starts with "v " it contains vertex information
    if(currentLine[0] == 'v' && currentLine[1] == ' ')
    {
      //Split up the line containing the vertex values into substrings
      vector <string> vertices;
      vertices = splitUpLine(currentLine, ' ');

      /*Takes the substrings converts them into doubles and adds them to the
      lists of X, Y, and Z vertices.*/
      Xvertices.push_back(str2Num(vertices[1]));
      Yvertices.push_back(str2Num(vertices[2]));
      Zvertices.push_back(str2Num(vertices[3]));
    }
    /*if the line in the object file starts with "f" then it contains faces
    (lists of vertices that make geometry)*/
    else if(currentLine[0] == 'f')
    {
      vector <string> parceFace;
      vector <string> parceFace2;
      vector <string> parceFace3;

      //Splits the line in the file into substrings
      faces = splitUpLine(currentLine, ' ');

      /*Splits the substrings into smaller substrings in order to get the
      individual numbers*/
      parceFace = splitUpLine(faces[1], '/');
      parceFace2 = splitUpLine(faces[2], '/');
      parceFace3 = splitUpLine(faces[3], '/');

      /*Converts the string numbers into doubles then stores them as the three
      vertices that will make up individual faces*/
      firstVertices.push_back(str2Num(parceFace[0]));
      secondVertices.push_back(str2Num(parceFace2[0]));
      thirdVertices.push_back(str2Num(parceFace3[0]));
    }
    else
    {
      // ignore evrything else in the object file for now
    }
  }
  /*Stores a copy of the current X, Y, and Z values. This will be necessary for
  later when switching between transformation modes*/
  currentXvert = Xvertices;
  currentYvert = Yvertices;
  currentZvert = Zvertices;
}

/***************************************************************************/
/*This function finds the center of the model and uses the center to calculate
the translation that will put the center of the model in the center of the
window*/
void centerModel()
{
  double xAverage = 0;
  double yAverage = 0;
  double zAverage = 0;

  for(unsigned int i = 0; i < Xvertices.size(); i++)
  {
    xAverage += Xvertices[i];
    yAverage += Yvertices[i];
    zAverage += Zvertices[i];
  }

  //These three variables represent the center of the variable
  xAverage /= Xvertices.size();
  yAverage /= Yvertices.size();
  zAverage /= Zvertices.size();

  /*Prepare the translation matrix for the translation to the center of the
  window*/
  xTrans = 250 - xAverage;
  yTrans = 250 - yAverage;
  zTrans = 250 - zAverage;
}

/***************************************************************************/
/*This function increases/decreases the size of the original model at the start
so that it can be seen easily */
void normalizeModel()
{
  double minX = 250;
  double minY = 250;
  double minZ = 250;

  double maxX = 250;
  double maxY = 250;
  double maxZ = 250;

  double maxDiff;

  //Finds the max and min vertices for X, Y, and Z
  for(unsigned int i = 0; i < Xvertices.size(); i++)
  {
    //Finding the min and max X values
    if(Xvertices[i] < minX)
    {
      minX = Xvertices[i];
    }
    if(Xvertices[i] > maxX)
    {
      maxX = Xvertices[i];
    }
    //Finding the min and max Y values
    if(Yvertices[i] < minY)
    {
      minY = Yvertices[i];
    }
    if(Yvertices[i] > maxY)
    {
      maxY = Yvertices[i];
    }
    //Finding the min and max Z values
    if(Zvertices[i] < minZ)
    {
      minZ = Zvertices[i];
    }
    if(Zvertices[i] > maxZ)
    {
      maxZ = Zvertices[i];
    }
  }

  /*Calculates the max difference between the max and min of any of X, Y,
  and Z values*/
  maxDiff = maxX - minX;
  if((maxY - minY) > maxDiff)
  {
    maxDiff = (maxY - minY);
  }
  if((maxZ - minZ) > maxDiff)
  {
    maxDiff = maxZ - minZ;
  }

  //Scale the model so that max Diff of any direction will be 350
  scaleFactor = 350/ maxDiff;
}
/***************************************************************************/

/*This function takes in a vertex and applies a translation to that vertex*/
vector<double> translateVertex(vector<double> vertexValues)
{
  vector<double> translatedVertex;
  vertexValues.push_back(1);

  //for the number of columns
  for(int i = 0; i < 4; i++)
  {
    //matrix that pertroms a translation based on the Trans variables
    double translateArray[4][4] = {
    {1, 0, 0, xTrans},
    {0, 1, 0, yTrans},
    {0, 0, 1, zTrans},
    {0, 0, 0, 1} };
    translatedVertex.push_back(0);
    //for the number of rows
    for(int j = 0; j < 4; j++)
    {
      /*multiplies the values from the passed in vertex against each element of
      the current row and takes the sum. This is done for every row. The result
      is a vertex that is translate based on the Tran variables*/
      translatedVertex[i] += translateArray[i][j] * vertexValues[j];
    }
  }
  return translatedVertex;
}

/***************************************************************************/
//This function takes in a vertex and preforms a scale transform on it
vector<double> scaleVertex(vector<double> vertexValues)
{
  vector<double> scaledVertex;
  vertexValues.push_back(1);

  //for the number of columns
  for(int i = 0; i < 4; i++)
  {
    double scaleArray[4][4] = { {scaleFactor, 0, 0, 0},
                                {0, scaleFactor, 0, 0},
                                {0, 0, scaleFactor, 0},
                                {0, 0, 0, scaleFactor} };
    scaledVertex.push_back(0);
    //for the number of rows
    for(int j = 0; j < 4; j++)
    {
      /*multiplies the values from the passed in vertex against each element of
      the current row and takes the sum. This is done for every row. The result
      is a vertex that is rotated based on scaleFactor*/
      scaledVertex[i] += scaleArray[i][j] * vertexValues[j];
    }
  }
  return scaledVertex;
}

/***************************************************************************/
//This function uses the current vertices and faces to perform a translation
void TranslateModel()
{
  //for the number of faces of the model
  for(unsigned int i = 0; i < firstVertices.size(); i++)
  {
    vector<double> vertex1;
    vector<double> vertex2;
    vector<double> vertex3;

    /*The firstVertices, secondVertices, and thirdVertices calls tell us which
    vertex to get. Xvertices and Yvertices grab that vertex. vertex1, vertex2
    and vertex3 temporarily store the vertices so that they can be passed into
    draw_triangle()*/
    vertex1.push_back(Xvertices[firstVertices[i] - 1]);
    vertex2.push_back(Xvertices[secondVertices[i] - 1]);
    vertex3.push_back(Xvertices[thirdVertices[i] - 1]);

    vertex1.push_back(Yvertices[firstVertices[i] - 1]);
    vertex2.push_back(Yvertices[secondVertices[i] - 1]);
    vertex3.push_back(Yvertices[thirdVertices[i] - 1]);

    vertex1.push_back(Zvertices[firstVertices[i] - 1]);
    vertex2.push_back(Zvertices[secondVertices[i] - 1]);
    vertex3.push_back(Zvertices[thirdVertices[i] - 1]);

    //Takes the vertices and performs a translation
    vertex1 = translateVertex(vertex1);
    vertex2 = translateVertex(vertex2);
    vertex3 = translateVertex(vertex3);

    //Draws a face with the new vertices
    draw_triangle(vertex1[0], vertex1[1], vertex2[0], vertex2[1], vertex3[0], vertex3[1]);

    /*This stores all current vertices. This will be used for a bug that occurs
    when switching between transformation mode*/
    currentXvert[firstVertices[i] - 1] = vertex1[0];
    currentXvert[secondVertices[i] - 1] = vertex2[0];
    currentXvert[thirdVertices[i] - 1] = vertex3[0];

    currentYvert[firstVertices[i] - 1] = vertex1[1];
    currentYvert[secondVertices[i] - 1] = vertex2[1];
    currentYvert[thirdVertices[i] - 1] = vertex3[1];

    currentZvert[firstVertices[i] - 1] = vertex1[2];
    currentZvert[secondVertices[i] - 1] = vertex2[2];
    currentZvert[thirdVertices[i] - 1] = vertex3[2];
  }
}
/***************************************************************************/
//Takes in a vertex and performs a rotation on it
vector<double> rotateVertex(vector<double> vertexValues)
{
  vector<double> rotatedVertex;
  vertexValues.push_back(1);

  //for the number of columns
  for(int i = 0; i < 4; i++)
  {
    //This branch is for when the rotation is along the horizontal axis
    if(horizArray == true)
    {
      double horizRotationArray[4][4] =
      { {1, 0,               0,                0},
        {0, cos(horizTheta), -sin(horizTheta), 0},
        {0, sin(horizTheta), cos(horizTheta),  0},
        {0, 0,               0,                1} };

      rotatedVertex.push_back(0);
      //for the number of rowss
      for(int j = 0; j < 4; j++)
      {
        /*multiplies the values from the passed in vertex against each element of
        the current row and takes the sum. This is done for every row. The result
        is a vertex that is rotated based on theta*/
        rotatedVertex[i] += horizRotationArray[i][j] * vertexValues[j];
      }
    }
    //This branch is for when the rotation is along the vertical axis
    else
    {
      double vertRotationArray[4][4] =
      { {cos(vertTheta),  0, sin(vertTheta), 0},
        {0,               1, 0,              0},
        {-sin(vertTheta), 0, cos(vertTheta), 0},
        {0,               0, 0,              1} };

      rotatedVertex.push_back(0);
      //for the number of rows
      for(int j = 0; j < 4; j++)
      {
        /*multiplies the values from the passed in vertex against each element of
        the current row and takes the sum. This is done for every row. The result
        is a vertex that is rotated based on theta*/
        rotatedVertex[i] += vertRotationArray[i][j] * vertexValues[j];
      }
    }
  }
  return rotatedVertex;
}

/***************************************************************************/
//This function performs a rotation on the current model
void RotateModel()
{
  //for the number of faces
  for(unsigned int i = 0; i < firstVertices.size(); i++)
  {
    vector<double> vertex1;
    vector<double> vertex2;
    vector<double> vertex3;

    /*The firstVertices, secondVertices, and thirdVertices calls tell us which
    vertex to get. Xvertices and Yvertices grab that vertex. vertex1, vertex2
    and vertex3 temporarily store the vertices so that they can be passed into
    draw_triangle()*/
    vertex1.push_back(Xvertices[firstVertices[i] - 1]);
    vertex2.push_back(Xvertices[secondVertices[i] - 1]);
    vertex3.push_back(Xvertices[thirdVertices[i] - 1]);

    vertex1.push_back(Yvertices[firstVertices[i] - 1]);
    vertex2.push_back(Yvertices[secondVertices[i] - 1]);
    vertex3.push_back(Yvertices[thirdVertices[i] - 1]);

    vertex1.push_back(Zvertices[firstVertices[i] - 1]);
    vertex2.push_back(Zvertices[secondVertices[i] - 1]);
    vertex3.push_back(Zvertices[thirdVertices[i] - 1]);

    //Takes the vertices and performs a rotation
    vertex1 = rotateVertex(vertex1);
    vertex2 = rotateVertex(vertex2);
    vertex3 = rotateVertex(vertex3);

    //print out the translated face
    draw_triangle(vertex1[0], vertex1[1], vertex2[0], vertex2[1], vertex3[0], vertex3[1]);

    /*This stores all current vertices. This will be used for a bug that occurs
    when switching between transformation mode*/
    currentXvert[firstVertices[i] - 1] = vertex1[0];
    currentXvert[secondVertices[i] - 1] = vertex2[0];
    currentXvert[thirdVertices[i] - 1] = vertex3[0];

    currentYvert[firstVertices[i] - 1] = vertex1[1];
    currentYvert[secondVertices[i] - 1] = vertex2[1];
    currentYvert[thirdVertices[i] - 1] = vertex3[1];

    currentZvert[firstVertices[i] - 1] = vertex1[2];
    currentZvert[secondVertices[i] - 1] = vertex2[2];
    currentZvert[thirdVertices[i] - 1] = vertex3[2];
  }
}

/***************************************************************************/

//This function preforms a scale transformation based on current vertices
void ScaleModel()
{
  //for the number of faces
  for(unsigned int i = 0; i < firstVertices.size(); i++)
  {
    vector<double> vertex1;
    vector<double> vertex2;
    vector<double> vertex3;

    /*The firstVertices, secondVertices, and thirdVertices calls tell us which
    vertex to get. Xvertices and Yvertices grab that vertex. vertex1, vertex2
    and vertex3 temporarily store the vertices so that they can be passed into
    draw_triangle()*/
    vertex1.push_back(Xvertices[firstVertices[i] - 1]);
    vertex2.push_back(Xvertices[secondVertices[i] - 1]);
    vertex3.push_back(Xvertices[thirdVertices[i] - 1]);

    vertex1.push_back(Yvertices[firstVertices[i] - 1]);
    vertex2.push_back(Yvertices[secondVertices[i] - 1]);
    vertex3.push_back(Yvertices[thirdVertices[i] - 1]);

    vertex1.push_back(Zvertices[firstVertices[i] - 1]);
    vertex2.push_back(Zvertices[secondVertices[i] - 1]);
    vertex3.push_back(Zvertices[thirdVertices[i] - 1]);

    //Takes the vertices of the face and performs a scale on them
    vertex1 = scaleVertex(vertex1);
    vertex2 = scaleVertex(vertex2);
    vertex3 = scaleVertex(vertex3);

    //Draws the scaled face
    draw_triangle(vertex1[0], vertex1[1], vertex2[0], vertex2[1], vertex3[0], vertex3[1]);

    /*This stores all current vertices. This will be used for a bug that occurs
    when switching between transformation mode*/
    currentXvert[firstVertices[i] - 1] = vertex1[0];
    currentXvert[secondVertices[i] - 1] = vertex2[0];
    currentXvert[thirdVertices[i] - 1] = vertex3[0];

    currentYvert[firstVertices[i] - 1] = vertex1[1];
    currentYvert[secondVertices[i] - 1] = vertex2[1];
    currentYvert[thirdVertices[i] - 1] = vertex3[1];

    currentZvert[firstVertices[i] - 1] = vertex1[2];
    currentZvert[secondVertices[i] - 1] = vertex2[2];
    currentZvert[thirdVertices[i] - 1] = vertex3[2];
  }
}

/***************************************************************************/

void display ( void )   // Create The Display Function
{

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	      // Clear Screen

  //normalizes the model and draws it as it is coming from the input file
  if(stub == 1)
  {
    //prepares the scale matrix
    normalizeModel();
    //preforms a scale to normalize the model
    ScaleModel();
    //makes sure the transform centers the model
    stub = 5;
  }
  //Draws the model after the desired translation has occured
  else if(stub == 2)
  {
    TranslateModel();
  }
  //Draws the model after the desired rotation has occured
  else if(stub == 3)
  {
    RotateModel();
  }
  //Draws the model after the desired scale has occured
  else if(stub == 4)
  {
    ScaleModel();
  }
  //Centers the model
  else if(stub == 5)
  {
    //These retain the scale that normalized the model
    Xvertices = currentXvert;
    Yvertices = currentYvert;
    Zvertices = currentZvert;
    //Prepares the translation matrix
    centerModel();
    //Translates the model to the middle of the window
    TranslateModel();
  }

  glutSwapBuffers();                                      // Draw Frame Buffer
}

/***************************************************************************/
void mouse(int button, int state, int x, int y)
{
/* This function I finessed a bit, the value of the printed x,y should
   match the screen, also it remembers where the old value was to avoid multiple
   readings from the same mouse click.  This can cause problems when trying to
   start a line or curve where the last one ended */
        static int oldx = 0;
        static int oldy = 0;
	int mag;

	y *= -1;  //align y with mouse
	y += 500; //ignore
	mag = (oldx - x)*(oldx - x) + (oldy - y)*(oldy - y);
	if (mag > 20) {
		printf(" x,y is (%d,%d)\n", x,y);
	}
	oldx = x;
	oldy = y;
	x_last = x;
	y_last = y;
}

/***************************************************************************/
void keyboard ( unsigned char key, int x, int y )  // Create Keyboard Function
{
  int i = 0;
	switch ( key ) {
		case 27:              // When Escape Is Pressed...
			exit ( 0 );   // Exit The Program
			break;
	        case '1':             // stub for new screen
		        printf("New screen\n");
			break;
    /*case 'z':
      ObtainFileFromUser();
      ParseFile();
      stub = 1;
      break;*/
    /*When the t key is pressed enter the translation mode. In order to maintain
    the position of the model from previous transformations the global variables
    that control translation (xTrans, yTrans, and zTrans) are reset and the x,
    y, and z vertices are updated on the lasts transformations current
    position.*/
    case 't':
      stub = 2;
      xTrans = 0;
      yTrans = 0;
      zTrans = 0;

      Xvertices = currentXvert;
      Yvertices = currentYvert;
      Zvertices = currentZvert;

      break;
    /*When the r key is pressed enter the rotation mode. In order to maintain
    the position of the model from previous transformations the global variables
    that control rotation (horizTheta and vertTheta) are reset and the x, y,
    and z vertices are updated on the lasts transformations current position.*/
    case 'r':
      stub = 3;
      horizTheta = 0;
      vertTheta = 0;
      Xvertices = currentXvert;
      Yvertices = currentYvert;
      Zvertices = currentZvert;
      break;
    /*When the e key is pressed enter the scale mode. In order to maintain
    the position of the model from previous transformations the global variable
    that control translation (scaleFactor) and the x, y, and z vertices are
    updated on the lasts transformations current position.*/
    case 'e':
      stub = 4;
      scaleFactor = 1;
      Xvertices = currentXvert;
      Yvertices = currentYvert;
      Zvertices = currentZvert;
      break;

    /*The "w" key can do many things in this program so when the "w" key is pressed
    we have to know which transformation mode we are in to perform the correct
    action*/
    case 'w':
      //If we are in translate mode move the model up by 10
      if(stub == 2)
      {
        yTrans += 10;
      }
      //If we are in rotate mode rotate the model horizontally by 0.03 radians
      else if(stub == 3)
      {
        /*This is too fix a bug that occurs when switching from horizontal and
        vertical rotation*/
        if(i == 0)
        {
          horizTheta = 0.03;
        }
        Xvertices = currentXvert;
        Yvertices = currentYvert;
        Zvertices = currentZvert;
        //this tells rotateVertex() which rotation matrix it needs to use
        horizArray = true;
      }
      //if we are in scale mode shrink the size of the model by a factor of 0.1
      else if(stub == 4)
      {
        scaleFactor -= 0.1;
      }
      break;
    /*The a key can do many things in this program so when the "a" key is
    pressed we have to know which transformation mode we are in to perform the
    correct action*/
    case 'a':
      //If we are in translate mode move the model down by 10
      if(stub == 2)
      {
        xTrans -= 10;
      }
      //If we are in rotate mode rotate the model vertically by 0.03 radians
      else if(stub == 3)
      {
        /*This is too fix a bug that occurs when switching from horizontal and
        vertical rotation*/
        if(i == 0)
        {
          vertTheta = 0.03;
        }
        Xvertices = currentXvert;
        Yvertices = currentYvert;
        Zvertices = currentZvert;
        //This tells retateVertex() which rotation matrix it needs to use
        horizArray = false;
      }
      //If we are in scale mode increase the size of the model by factor of 0.03
      else if(stub == 4)
      {
        scaleFactor += 0.1;
      }
      break;
    /*The s key can do many things in this program so when the "s" key is
    pressed we have to know which transformation mode we are in to perform the
    correct action*/
    case 's':
      //If we are in translate mode move the model down by 10
      if(stub == 2)
      {
        yTrans -= 10;
      }
      //If we are in rotate mode rotate the model horizontally by -0.03 radians
      else if(stub == 3)
      {
        /*This is too fix a bug that occurs when switching from horizontal and
        vertical rotation*/
        if(i == 0)
        {
          horizTheta = -0.03;
        }
        Xvertices = currentXvert;
        Yvertices = currentYvert;
        Zvertices = currentZvert;
        //This tells retateVertex() which rotation matrix it needs to use
        horizArray = true;
      }
      //If we are in scale mode shrink the size of the model by factor of 0.03
      else if(stub == 4)
      {
        scaleFactor -= 0.1;
      }
      break;
    /*The d key can do many things in this program so when the "d" key is
    pressed we have to know which transformation mode we are in to perform the
    correct action*/
    case 'd':
      //If we are in translate mode move the model up by 10
      if(stub == 2)
      {
        xTrans += 10;
      }
      //If we are in rotate mode rotate the model vertically by -0.03 radians
      else if(stub == 3)
      {
        /*This is too fix a bug that occurs when switching from horizontal and
        vertical rotation*/
        if(i == 0)
        {
          vertTheta = -0.03;
        }
        Xvertices = currentXvert;
        Yvertices = currentYvert;
        Zvertices = currentZvert;
        //This tells retateVertex() which rotation matrix it needs to use
        horizArray = false;
      }
      //If we are in scale mode increase the size of the model by factor of 0.1
      else if(stub == 4)
      {
        scaleFactor += 0.1;
      }
      break;

		default:
			break;
      i++;
	}
}
/***************************************************************************/

int main (int argc, char *argv[])
{
/* This main function sets up the main loop of the program and continues the
   loop until the end of the data is reached.  Then the window can be closed
   using the escape key.						  */

	glutInit            ( &argc, argv );
  filename = argv[1];
  ObtainFileFromUser();
  ParseFile();
       	glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowSize  ( 500,500 );
	glutCreateWindow    ( "Computer Graphics" );
	glutDisplayFunc     ( display );
	glutIdleFunc	    ( display );
	glutMouseFunc       ( mouse );
	glutKeyboardFunc    ( keyboard );

        init_window();				             //create_window

	glutMainLoop        ( );                 // Initialize The Main Loop

}
