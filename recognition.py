import vision
import matplotlib.pyplot as plt
import numpy as np

raw = vision.rawImg(480, 360)
grey = vision.greyscale(raw)
blr = vision.gaussianBlur(grey, 3, 1.5)
sbl = vision.sobel(blr)

boardBorder = 15    #thickness of border in pixels
lineThresh = 20000  #sum of pixels in row or comlumn to count as a line
sqrBorder = 8       #border around each grid detection square
blackThreshold = 20 #threshold to apply to grey to locate black pieces

#piece detection
allPieceThresh = 5000   #number of edge pixels in region to count as piece
blackPieceThresh = 10 #number of black threshed pixels in region to count as black piece


boardCols =[]
boardRows = []
inRol = False
inCol = False

for c in range(sbl.shape[1]):
    if np.sum(sbl[:, c]) > lineThresh:
        if not inCol:
            inCol = True
            boardCols.append(c)
    else:
        inCol = False

for r in range(sbl.shape[0]):
    if np.sum(sbl[r]) > lineThresh:
        if not inRow:
            inRow = True
            boardRows.append(r)
    else:
        inRow = False

leftEdge = boardCols[0] + boardBorder
rightEdge = boardCols[-1] - boardBorder
topEdge = boardRows[0] + boardBorder
bottomEdge = boardRows[-1] - boardBorder

sqrSize = round(((rightEdge-leftEdge) + (bottomEdge-topEdge))/16) #dividing by 2 to get average then 8 as 8 squares

edgeRois = np.zeros((8,8))
threshRois = np.zeros((8,8))

for y in range(8):
    for x in range(8):
        l = leftEdge + ( x    * sqrSize) + sqrBorder
        r = leftEdge + ((x+1) * sqrSize) - sqrBorder
        t = topEdge  + ( y    * sqrSize) + sqrBorder
        b = topEdge  + ((y+1) * sqrSize) - sqrBorder
        edgeRoi = sbl[t:b, l:r] 
        threshRoi = np.logical_not(vision.threshold(blr, blackThreshold))[t:b, l:r]
        edgeRois[y][x] = np.sum(edgeRoi)
        threshRois[y][x] = np.sum(threshRoi)
        sbl[t:b,l:r] = 255


print("all pieces")
[print(''.join([str(int(c)).rjust(2) for c in r])) for r in (edgeRois > allPieceThresh)]

print("black pieces")
[print(''.join([str(int(c)).rjust(2) for c in r])) for r in (threshRois > blackPieceThresh)]

sbl[:, leftEdge] = 255
sbl[:, rightEdge] = 255
sbl[topEdge] = 255
sbl[bottomEdge] = 255

plt.imshow(raw)
#plt.imshow(sbl, cmap="gray")
plt.show()
