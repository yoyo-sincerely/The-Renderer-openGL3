from PIL import Image
import os
im = Image.open('F:\SchoolWork\SchoolWork\The Renderer openGL3\examples\data\images\example.jpg')
im.show()
bounds = (0,0,128,128)
cutoutIm = im.crop (bounds)
cutoutIm.show()
cutoutIm.save('crop.jpg')
print(os.getcwd())