This program takes a foreground image and a background image and seeks to make a new image with the background image as the background of the foreground image. This operates much like green screens in the entertainment industry. There are two methods to create this new image: 1) a user-defined threshold number which tells the program at what chroma distance should the given pixel be from the foreground image or the background image; and 2) an program-determined optimal threshold.

All the provided foreground images are provided in the "front" directory in the present working directory. You have:

astro.bmp: an Astronaut
bird_gs.bmp: a bird
colbert_gs.bmp: Steven Colbert with a sweet guitar
faded.bmp: colbert_gs.bmp just with a faded tint like an old photo.
smurfbert.bmp: colbert_gs.bmp with a heavy ocean blue tint.
trebloc.bmp: colbert_gs.bmp with a weird color scheme and mirrored.

All the provided background images are provided in the "back" directory in the present working directory. You have:

campus.bmp: The front entrance of the Bovard Building on USC campus.
pipe.bmp: Screen from a retro Mario video game.
space.bmp: Outer Space with stars
village.bmp: Cartoon mushroom village as if to suggest one being under the influence of LSD

 Notes:
 This program performs the chroma key operation on an input 
 using two different methods.

 Method 1 Utilize a user-defined distance threshold from the
          chromakey value as a discriminator

 Method 2 Devise a method that to determine the chromakey mask
          that doesn't require a user-input threshold

Usage: ./chromakey foreground.bmp background.bmp threshold method1.bmp method2.bmp

To compile, use "make" and run as "Usage" suggests.

