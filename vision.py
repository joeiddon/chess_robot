"""Computer Vision library for Python-3.x

Support only for RPI camera module"""

import numpy as np
import math

def rawImg(x, y):
    """Returns numpy array of the raw camera data of size 1663x1232"""
    try:
        import picamera
    except ImportError:
        print("sorry, picamera is not installed")
        return
    x = int(32*math.ceil(x/32))
    y = int(16*math.ceil(y/16))
    with picamera.PiCamera() as camera:
        camera.resolution = (x, y)
        output = np.empty((y, x, 3), dtype="uint8") 
        camera.capture(output, "rgb")
        return output

def greyscale(img):
    "Returns img conveted to greyscale"
    return np.dot(img, [0.2126, 0.7152, 0.0722])
    
def threshold(img, thresh):
    "Returns a numpy array of boolean values"
    return img > thresh

def gaussianKernelOld(size, sigma):
    "Returns Kernel matrix required for Gaussian Blur"
    bl = int((size-1)/2)
    kernel = np.array([[ (1/(2*math.pi*sigma**2)) * math.e ** ((-1*(x**2+y**2))/(2*sigma**2)) for x in range(-bl,bl+1)] for y in range(-bl,bl+1)])
    return kernel / np.sum(kernel)  #np.array([[v/weightSum for v in r] for r in weightMatrix])

def gaussianKernel(size, sigma, twoDimensional=True):
    if twoDimensional:
        kernel = np.fromfunction(lambda x, y: (1/(2*math.pi*sigma**2)) * math.e ** ((-1*((x-(size-1)/2)**2+(y-(size-1)/2)**2))/(2*sigma**2)), (size, size))
    else:
        kernel = np.fromfunction(lambda x: math.e ** ((-1*(x-(size-1)/2)**2) / (2*sigma**2)), (size,))
    return kernel / np.sum(kernel)

def gaussianBlurOld(img, kSize, kSigma):
    "Returns a numpy array of the greyscale img gaussian blurred with the size and sigma vals WARNING: reduces dimensions"
    kernel = gaussianKernel(kSize, kSigma)
    d = int((kSize-1)/2)
    gaussian = np.zeros((img.shape[0]-2*d, img.shape[1]-2*d))
    for y in range(d, img.shape[0]-d):
        for x in range(d, img.shape[1]-d):
            gaussian[y-d][x-d] = np.sum(np.multiply(img[y-d:y+d+1, x-d:x+d+1], kernel))
            #for yy in range(kSize):
            #    for xx in range(kSize):
            #        gaussian[y][x] += img[(y-d)+yy][(x-d)+xx] * kernel[-d+yy][-d+xx]
    return gaussian

def gaussianBlur(img, kSize, kSigma):
    kernel = gaussianKernel(kSize, kSigma, False)
    gausX = np.zeros((img.shape[0], img.shape[1] - kSize + 1), dtype="float64") # kernel[0][0] * img[:, :img.shape[1]-kSize+1] #np.zeros((img.shape[0]-kSize-1, img.shape[1]-kSize-1))
    for i, v in enumerate(kernel):
        gausX += v * img[:, i : img.shape[1] - kSize + i + 1]
    gausY = np.zeros((gausX.shape[0] - kSize + 1, gausX.shape[1]))  #gausX[:gausX.shape[0]-kSize+1]
    for i, v in enumerate(kernel):
        gausY += v * gausX[i : img.shape[0]  - kSize + i + 1]
    return gausY

#def gaussianBlur(img, kSize, kSigma):
#    kernel = getKernel(kSize, kSigma)
#    d = int((kSize-1)/2)
#    gausX = np.zeros


def sobelX(img):
    sblH = img[:,2:] - img[:,:-2]
    sblV = sblH[:-2] + 2*sblH[1:-1] + sblH[2:]
    return sblV

def sobelY(img):
    sblH = img[:,2:] + 2*img[:,1:-1] + img[:,:-2]
    sblV = sblH[2:] - sblH[:-2]
    return sblV

def sobel(img, simple=False):
    "Returns a numpy array of the edges of a greyscale img, simple parameter sets whether uses absolute rather than squaring"
    if simple:
        return abs(sobelX(img)) + abs(sobelY(img))
    return (sobelX(img)**2 + sobelY(img)**2) ** 0.5


def sobelOld(img, seperate=False):
    "Returns a numpy array of the edges as gradients or if seperate, seperate gradients in the form [gx, gy, g] from the greyscale img"
    xKernel = np.array([[-1,0,1],[-2,0,2],[-1,0,1]])
    yKernel = np.array([[-1,-2,-1],[0,0,0],[1,2,1]])
    sobelled = np.zeros((img.shape[0]-2, img.shape[1]-2, 3), dtype="uint8")
    for y in range(1, img.shape[0]-1):
        for x in range(1, img.shape[1]-1):
            gx = np.sum(np.multiply(img[y-1:y+2, x-1:x+2], xKernel))
            gy = np.sum(np.multiply(img[y-1:y+2, x-1:x+2], yKernel))
            g = abs(gx) + abs(gy) #math.sqrt(gx ** 2 + gy ** 2)
            if seperate:
               sobelled[y-1][x-1] = [gx, gy, g]
            else:
               g = g if g > 0 and g < 255 else (0 if g < 0 else 255)
               sobelled[y-1][x-2] = g
    return sobelled


def canny(img):
    "Returns a numpy array of thinned edges with double threshold from sobel operators"
    sbl = sobel(img, True)
    can = np.zeros((img.shape[0], img.shape[1]), dtype="uint8")
    for y in range(1, sbl.shape[0]-1):
        for x in range(1,sbl.shape[1]-1):
            theta = math.atan2(sbl[y][x][0], sbl[y][x][1]) * (180 / math.pi)
            edge = int(45 * round(theta/45))
            if edge == 0:
                can[y][x] = sbl[y][x][2] if sbl[y][x][2] > sbl[y][x-1][2] and sbl[y][x][2] > sbl[y][x+1][2] else 0
            elif edge == 45:
                can[y][x] = sbl[y][x][2] if sbl[y][x][2] > sbl[y-1][x-1][2] and sbl[y][x][2] > sbl[y+1][x+1][2] else 0
            elif edge == 90:
                can[y][x] = sbl[y][x][2] if sbl[y][x][2] > sbl[y-1][x][2] and sbl[y][x][2] > sbl[y+1][x][2] else 0
            elif edge == 135:
                can[y][x] = sbl[y][x][2] if sbl[y][x][2] > sbl[y-1][x+1] and sbl[y][x][2] > sbl[y+1][x-1][2] else 0 

    return can
