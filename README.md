# wordlebot
cmd line machine player of wordle.

The program guesses words and, currently, a human types in the wordle results of playing the word as a string of '-','y', and 'g' chracters, meaning gray, yellow, and green respectively.

This code is C++17 using range-v3 ranges, as a visual studio solution. If there is interest I can turn into a CMake project and I'd also like to switch it to use C++23 ranges eventually (but currently just putting this here to use GitHub as source control and making it public, because, why not?)

Right now the big problem with the way it plays is that Wordlebot doesnt take word frequency into account at all. it is concerned only with how likely words it guesses are to give it lots of yellow and green tiles, which is fine in the beginning but when you only have a few squares that are not green it should transition into only caring about finding the most common word that fits the pattern, as Wordle likes common words. I plan to evebutally add this transition of concern eventually.
