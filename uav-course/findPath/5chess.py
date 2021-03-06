'''from tkinter import *

app = Tk()

#通过event形参来获取对应事件描述
def callback(event): 
    print("当前位置：",event.x,event.y)

#创建框架，窗口尺寸
frame = Frame(app, width = 200, height = 200)
#frame.bind("<Motion>",callback)
frame.bind("<Button-1>",callback)
frame.bind("<Button-2>",callback)
frame.bind("<Button-3>",callback)
frame.pack()
#<Button-1>Button：表示鼠标的点击事件 “—”左边是事件本身，右边是事件描述
#1：表示左键 2：中间键的滚轮点击 3：右键

mainloop()'''

'''from tkinter import *

root=Tk()
w=Canvas(root,width=400,height=200,background='white')
w.pack()

def paint(event):
    #event.x 鼠标左键的横坐标
    #event.y 鼠标左键的纵坐标
    x1,y1=(event.x-1),(event.y-1)
    x2,y2=(event.x+1),(event.y+1)
    w.create_oval(x1,y1,x2,y2,fill='red')
#鼠标左键一点，就画出了一个小的椭圆

def call(event):
    print(event)

#画布与鼠标左键进行绑定
w.bind("<B1-Motion>",paint)
w.bind("<Button-1>",call)

mainloop()'''

from tkinter import *
import random

BOARD_WIDTH = 535
BOARD_HEIGHT = 536
BOARD_SIZE = 15
# 定义棋盘坐标的像素值和棋盘数组之间的偏移距。
X_OFFSET = 21
Y_OFFSET = 23
# 定义棋盘坐标的像素值和棋盘数组之间的比率。
X_RATE = (BOARD_WIDTH - X_OFFSET * 2) / (BOARD_SIZE - 1)
Y_RATE = (BOARD_HEIGHT - Y_OFFSET * 2) / (BOARD_SIZE - 1)
BLACK_CHESS = "●"
WHITE_CHESS = "○"
board = []
# 把每个元素赋为"╋"，代表无棋
for i in range(BOARD_SIZE) :
    row = ["╋"] * BOARD_SIZE
    board.append(row)
# 创建窗口
root = Tk()
# 禁止改变窗口大小
root.resizable(width=False, height=False)
# 修改图标
#root.iconbitmap('images/fklogo.ico')
# 设置窗口标题
root.title('五子棋')
# 创建并添加Canvas
cv = Canvas(root, background='white',
    width=BOARD_WIDTH, height=BOARD_HEIGHT)
cv.pack()
bm = PhotoImage(file="images/0.GIF")
cv.create_image(BOARD_HEIGHT/2 + 1, BOARD_HEIGHT/2 + 1, image=bm)
selectedbm = PhotoImage(file="images/black.png")
# 创建选中框图片，但该图片默认不在棋盘中
selected = cv.create_image(-100, -100, image=selectedbm)
def move_handler(event):
    # 计算用户当前的选中点，并保证该选中点在0～14之间
    selectedX = max(0, min(round((event.x - X_OFFSET) / X_RATE), 14))
    selectedY = max(0, min(round((event.y - Y_OFFSET) / Y_RATE), 14))
    # 移动红色选择框
    cv.coords(selected,(selectedX * X_RATE + X_OFFSET,
        selectedY * Y_RATE + Y_OFFSET))
black = PhotoImage(file="images/black.png")
white = PhotoImage(file="images/white.png")
def click_handler(event):
    # 计算用户的下棋点，并保证该下棋点在0～14之间
    userX = max(0, min(round((event.x - X_OFFSET) / X_RATE), 14))
    userY = max(0, min(round((event.y - Y_OFFSET) / Y_RATE), 14))
    # 当下棋点没有棋子时，才能下棋子，用户才能下棋子
    if board[userY][userX] == "╋":
        cv.create_image(userX * X_RATE + X_OFFSET, userY * Y_RATE + Y_OFFSET,
            image=black)
        board[userY][userX] = "●"
        while(True):
            comX = random.randint(0, BOARD_SIZE - 1)
            comY = random.randint(0, BOARD_SIZE - 1)
            # 如果电脑要下棋的点没有棋子时，才能让电脑下棋
            if board[comY][comX] == "╋": break
        cv.create_image(comX * X_RATE + X_OFFSET, comY * Y_RATE + Y_OFFSET,
            image=white)
        board[comY][comX] = "○"
def leave_handler(event):
    # 将红色选中框移出界面
    cv.coords(selected, -100, -100)
# 为鼠标移动事件绑定事件处理函数
cv.bind('<Motion>', move_handler)
# 为鼠标点击事件绑定事件处理函数
cv.bind('<Button-1>', click_handler)
# 为鼠标移出事件绑定事件处理函数
cv.bind('<Leave>', leave_handler)
root.mainloop()

