//
//  main.cpp
//  OpenGL绘制正方形
//
//  Created by 姜云珑 on 2020/7/5.
//  Copyright © 2020 jyl. All rights reserved.
//

// 着色器管理器
#include "GLShaderManager.h"

#include "GLTools.h"

#include <GLUT/GLUT.h>

bool normal;

GLShaderManager shaderManager;
// 正方形批次容器
GLBatch quaBatch;
// 距离坐标轴的边长
GLfloat blockSize = 0.1;
// 正方形4个顶点坐标
GLfloat vVerts[] = {
    -blockSize, -blockSize, 0.0f,
    blockSize, -blockSize, 0.0f,
    blockSize, blockSize, 0.0f,
    -blockSize, blockSize, 0.0f
};

// 中心点
GLfloat xPos = 0.0f;
GLfloat yPos = 0.0f;

/*
 * 在窗口大小改变时，接收新的宽度&高度。
 */
void changeSize(int w, int h) {
    
    // 通常x、y都为0
    glViewport(0, 0, w, h);
}

// 每一次图形移动都会调用
void renderScene() {
    
    /*
     * 清除一个或者一组特定的缓冲区
     * 缓冲区是一块存在图像信息的储存空间，RGBA通常一起作为颜色缓冲区或像素缓冲区引用
     * OpenGL 中不止一种缓冲区（颜色缓冲区、深度缓冲区、模板缓冲区）
     * GL_COLOR_BUFFER_BIT 颜色缓冲区
     * GL_DEPTH_BUFFER_BIT 深度缓冲区
     * GL_STENCIL_BUFFER_BIT 模块缓冲区
     ****/
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // 设置画笔颜色
    GLfloat vRed[] = {1.0, 0.0, 0.0, 1.0};
    if (normal) {
        /*
         * 传递到存储着色器(固定管线)
         * GLT_SHADER_IDENTITY 顶点着色器
         */
        shaderManager.UseStockShader(GLT_SHADER_IDENTITY, vRed);
    } else {
        /*
         * 矩阵变换 定义一个4*4的矩阵 M3DMatrix44f 是一个一维数组
         * 从类型就能看出来 44代表4*4
         * 红 绿 蓝 缩放比例
         */
        M3DMatrix44f mTransfromMatrix;
        /*
         * 得到矩阵变化后的坐标
         **/
        m3dTranslationMatrix44(mTransfromMatrix, xPos, yPos, 0.0f);
        /*
         * GLT_SHADER_FLAT 平面着色器
         */
        shaderManager.UseStockShader(GLT_SHADER_FLAT, mTransfromMatrix, vRed);
    }
    // 提交着色器
    quaBatch.Draw();
    // 将后台缓冲区进行渲染，完成后交给前台
    glutSwapBuffers();
}

void setupRC() {
     
    // 设置背景颜色
    glClearColor(1.0, 0.7, 0.7, 1.0);
    
    // 初始化着色器， 没有着色器是没有办法渲染的
    shaderManager.InitializeStockShaders();
    
    // 批次提交
    quaBatch.Begin(GL_TRIANGLE_FAN, 4);
    quaBatch.CopyVertexData3f(vVerts);
    quaBatch.End();
};

// 普通做法，通过每一个顶点坐标计算获取最终移动后的顶点坐标 相当于以点即点 因为要换算每一个顶点坐标
void normalDisplay(int key, GLfloat stepSize) {
    
    // 取d点坐标 通过d点坐标换算a、b、c坐标
     GLfloat blockX = vVerts[0];
     GLfloat blockY = vVerts[10];
    
     switch (key) {
         case GLUT_KEY_UP:
             blockY += stepSize;
             break;
         case GLUT_KEY_DOWN:
             blockY -= stepSize;
             break;
         case GLUT_KEY_LEFT:
             blockX -= stepSize;
             break;
         case GLUT_KEY_RIGHT:
             blockX += stepSize;
             break;
         default:
             break;
     }
     
     // 左移
     if (blockX < -1.0f) {
         blockX = -1.0f;
     }
     // 右移 减去2倍边长
     if (blockX > (1.0 - blockSize * 2)) {
         blockX = 1.0 - blockSize * 2;
     }
     // 上移
     if (blockY > 1.0f) {
         blockY = 1.0f;
     }
     // 下移
     if (blockY < -1.0f + blockSize * 2) {
         blockY = -1.0f + blockSize * 2;
     }
     
     // 为啥不处理vVerts[2]/vVerts[5]/vVerts[8] 因为是z轴，z轴没有变化
     // a
     vVerts[0] = blockX;
     vVerts[1] = blockY - blockSize * 2;
     //b
     vVerts[3] = blockX + blockSize * 2;
     vVerts[4] = blockY - blockSize * 2;
     //c
     vVerts[6] = blockX + blockSize * 2;
     vVerts[7] = blockY;
     //d
     vVerts[9] = blockX;
     vVerts[10] = blockY;
     
     //需要重新提交渲染
     quaBatch.CopyVertexData3f(vVerts);
     glutPostRedisplay();
}

// 矩阵做法，通过一个中心点去换算全部的顶点坐标，相当于以点即面 不需要换算每一个顶点坐标
void matrixDisplay(int key, GLfloat stepSize) {
    
    switch (key) {
        case GLUT_KEY_UP:
            yPos += stepSize;
            break;
        case GLUT_KEY_DOWN:
            yPos -= stepSize;
            break;
        case GLUT_KEY_LEFT:
            xPos -= stepSize;
            break;
        case GLUT_KEY_RIGHT:
            xPos += stepSize;
            break;
        default:
            break;
    }
    
    // 左移
    if (xPos < -1.0f + blockSize) {
        xPos = -1.0f + blockSize;
    }
    // 右移
    if (xPos > 1.0 - blockSize) {
        xPos = 1.0 - blockSize;
    }
    // 上移
    if (yPos > 1.0f - blockSize) {
        yPos = 1.0f - blockSize;
    }
    // 下移
    if (yPos < -1.0f + blockSize) {
        yPos = -1.0f + blockSize;
    }
    
    glutPostRedisplay();
}

void dealKeys(int key, int x, int y) {
    /*
     * 可以监听键盘的案件很多包括单键、组合键等
     * 我们只需要监听上下左右就可以
     * 加上边界测试
     */
    
    // 移动多远
    GLfloat stepSize = 0.025;
    
    /*
     * d-----c
     * |     |
     * |     |
     * a-----b
     *
     * a和d、b和c x轴坐标相等 a和b、c和d y轴坐标相等
     */
   if (normal) {
         normalDisplay(key, stepSize);
     } else {
         matrixDisplay(key, stepSize);
     }
}

int main(int argc, char *argv[]) {
    
    // 区分顶点坐标法和矩阵变换法
    normal = false;
    
    /*
     * 在Mac OS X中，这个程序将当前工作文件夹改为应用程序捆绑包中的`/Resource`文件夹。`GLUT`的优先设定自动进行了这个中设置，但是这样中方法更加安全。
     */
    gltSetWorkingDirectory(argv[0]);
    
    glutInit(&argc, argv);
    
    /*
     * 设置双缓冲区
     * GLUT_DOUBLE 双缓冲窗口
     * GLUT_RGBA RGBA颜色模式
     * GLUT_DEPTH 深度测试
     * GLUT_STENCIL 模板缓冲区
     */
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
    
    // 设置窗口大小
    glutInitWindowSize(1000, 1000);
    // 设置窗口名称
    glutCreateWindow("Qua");
    
    // 注册重塑函数
    glutReshapeFunc(changeSize);
    // 注册显示函数
    glutDisplayFunc(renderScene);
    
    // 注册键盘监听
    glutSpecialFunc(dealKeys);
    
    /*
     * 初始化一个GLEW库，确保OpenGL API对程序完全可用
     * 在渲染之前， 检查驱动程序的初始化过程没有问题
     */
    GLenum status = glewInit();
    if (GLEW_OK != status) {
        
        printf("GLEW Error:%s\n",glewGetErrorString(status));
        return 1;
    }
    
    // 设置渲染环境
    setupRC();
    // 开启事件循环
    glutMainLoop();
    
    return 0;
}


