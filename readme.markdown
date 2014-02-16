# TipToe: Data Transmission Over Wireless Carrier Audio Channel

The Tiptoe library is an effort to emulate the results found in the
Hermes project, which aimed to transmit data over our wireless
carrier's audio channel.

## Tiptoe Vs Hermes
There is a fundamental difference in the Tiptoe and Hermes protocol.
Hermes operated with three frequencies, while Tiptoe used five.  This
enabled the transcoding to cause a smaller increase in package size.
In particular, Hermes had a 2X post-transcoding size, while Tiptoe had
1.5X post-transcoding size.  The increase in frequency use was
acceptable to the telco codecs as long as frequency jumping did not
occur, where jumping is moving to a frequency that was not neighboring
the current frequency.  More details about this protocol are found in
the comments in the code.

The Hermes paper is found here:
http://cs.nyu.edu/~jchen/publications/com31a-dhananjay.pdf

## Telco Codec's Distortion
The reason this project (and Hermes) exists is because we can not simply
take a modem and connect it to our cellphone.  The Telco codec mangles
our data to make it look like a voice stream.  This is well documented
in the Hermes paper, though I wanted to show the results here.  Listening to
these is not as useful as looking at the waveforms.  
[Here is a link to clean input data](https://s3.amazonaws.com/keithgould/tiptoe/clean_input.wav)  
[Here is a link to the resulting data after going through AT&T](https://s3.amazonaws.com/keithgould/tiptoe/output.wav)  
(Note that all carriers do this to varying degrees)  
  
### Here is the waveform comparison (input on the top):
![Audio Distortion](https://s3.amazonaws.com/keithgould/tiptoe/audio_compare.png)

## TipToe codebase
Most of the functionality is found in the utilities directory.  The
other root directories contain example apps that wrap the main
functionality.  Each wrapper app has a Makefile.

The toolchain expects an Apple Mac.  I used a MacBook Air.

At the very least you will need [port audio](http://www.portaudio.com/).  
You will also want [Eigen]
(http://eigen.tuxfamily.org/index.php?title=Main_Page) if you want to
use the Hamming error correction utility.

## Crypto
I was dabbling with incorporating crypto into this project, so you
will find mention of AES and the crypyo++ libraries.  They are not
relevant to the main purpose of the project, feel free to rip out
and/or ignore.
