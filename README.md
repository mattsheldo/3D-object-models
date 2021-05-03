# 3D-object-models

This is a class project I did for my computer graphics class. The program takes in an obj file which contains x, y, and z coordinates for the vertices that make up the object. All the vertices are connected by edges so that a basic 3D image is created. Once the image is created there are three different modes that can be used to manipulate the model. The three modes are rotation, translation, and Scaling.

## Try it out yourself!

Use Git to clone the repository and run the program. On your machine navigate to your desired folder and run the following command.

```bash
git clone 
```
Once you have the repository cloned on your machine you can run the following commands to compile and run the code.
There is a make file included in the repository so to compile all you need to do is type the make in the command line.

```bash
make 
```
Once the code is compiled you run it by using the following command.

```bash
./3Dmodeler <name_of_file>.obj
```
You can use any obj file when running the program but I have included 3 obj files for you to use.

## Usage

Once the program is compiled and run a window should pop up that displays the model. Once the model is displayed you can use the E, R, and T buttons to change modes. Once you have selected a mode you use the W, A, S, and D to manipulate the model.

### Scale Mode

By pressing E you enter the scale mode. When in scale mode the A and D keys increase the size of the model and the W and the S keys decrease the size of the model.

### Rotation Mode

By pressing R you enter the rotation mode. When in rotation mode the W key rotates the model upwards, the A key rotates the model left, the S key rotates the model downward, and the D key rotates the model right.

### Translation Mode

By Pressing T you enter the translation mode. When in translation mode the W key moves the model upwards, the A key moves the model left, the S key moves the model down, and the D key moves the model right.

You can switch back and forth between all modes at anytime so that you can combine rotations with translations and changes in scale.

### Example Using the whale.obj File
e

![image](https://user-images.githubusercontent.com/83359136/116893878-769ddf00-abff-11eb-86ca-c5bd58a1e34e.png)
