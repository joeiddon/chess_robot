import vision
import numpy as np

def recognise(display=False):
    """
    Operation:
     - determine where the chess board is
     - count the number of pixels in each grid square
     - if this is over a limit, say that there is a piece there
     - all the pieces which pass the black threshold are black
     - if they only pass the white threshold, they're white
     - returns a tuple of 8x8 numpy arrays for white and black pieces
    """

    #general
    sqr_padding = 8      #inner for each grid square
    line_col = 255

    #detection thresholds
    white_pix_thresh = 105 #threshold for pix to count as white
    black_pix_thresh = 50  #threshold for pix to count as black
    white_pix_no = 10      #number of white threshed pixels for piece
    black_pix_no = 10      #number of black threshed pixels for piece

    #pixels of each edge of chess board
    #(hard coded because board's position must be fixed anyway for arm)
    t_edge = 0
    b_edge = 246
    l_edge = 122
    r_edge = 370

    #size of one grid square in pixels
    sqr_sz = round(((r_edge-l_edge)+(b_edge-t_edge))/16)

    #get the images
    raw  = vision.raw_img(480, 360)
    grey = vision.greyscale(raw)
    #blr  = vision.gaussianBlur(grey, 3, 1.5)
    #sbl  = vision.sobel(blr)

    black_threshed = grey < black_pix_thresh
    white_threshed = grey < white_pix_thresh

    black_pieces = np.zeros((8,8), 'uint8')
    white_pieces = np.zeros((8,8), 'uint8')

    for y in range(8):
        for x in range(8):
            l = l_edge + ( x    * sqr_sz) + sqr_padding
            r = l_edge + ((x+1) * sqr_sz) - sqr_padding
            t = t_edge + ( y    * sqr_sz) + sqr_padding
            b = t_edge + ((y+1) * sqr_sz) - sqr_padding

            black_sqr = black_threshed[t:b,l:r]
            white_sqr = white_threshed[t:b,l:r]
            is_black_piece = np.sum(black_sqr) > black_pix_no
            is_white_piece = np.sum(white_sqr) > white_pix_no

            if is_white_piece:
                if is_black_piece:
                    black_pieces[y][x] = 1
                else:
                    white_pieces[y][x] = 1

            if display:
                grey[t:b,l] = line_col
                grey[t:b,r] = line_col
                grey[t,l:r] = line_col
                grey[b,l:r] = line_col

    if display:
        import matplotlib.pyplot as plt
        grey[:,l_edge] = \
        grey[:,r_edge] = \
        grey[t_edge]   = \
        grey[b_edge]   = line_col
        print('whites:  blacks:')
        [print(w,b) for w,b in zip((''.join(map(str,r)) for r in white_pieces),
                                   (''.join(map(str,r)) for r in black_pieces))]
        fig = plt.gcf()
        for i,a in enumerate(('grey', 'white_threshed', 'white_pieces', 'black_pieces')):
            ax = fig.add_subplot(2,2,i+1)
            ax.set_title(a)
            ax.imshow(locals()[a],'gray')

        plt.show()

    return (white_pieces, black_pieces)
