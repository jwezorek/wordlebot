# wordlebot
Command line machine player of Wordle ... The program guesses words and, currently, a human types in the results Wordle generates as a string of '-','y', and 'g' characters -- meaning gray, yellow, and green respectively.

This code is C++17 using range-v3 ranges, as a Visual Studio solution. If there is interest I can turn it into a CMake project and I'd also like to switch it to use C++23 ranges eventually (but currently just putting this here to use GitHub as source control and making it public, because, why not?)

Right now the big problem with the way it plays is that Wordlebot doesnt take word frequency into account at all. It is concerned only with how likely words its guesses are to yield lots of yellow and green, which is fine in the beginning but when it only has a few letters that are not green remaining it should transition into only caring about finding the most common word that fits the pattern, as Wordle likes common words. I plan to add this transition of concern eventually.

Sample session below, where the hidden word is 'verse'.

```
wordlebot guesses 'rates'
  => y--yy

wordlebot guesses 'senor'
  => yg--y

wordlebot guesses 'perse'
  => -gggg

wordlebot guesses 'verse'
  => ggggg

  4/6
```

(You can see the word frenquency problem in the above where it choose 'perse' over 'verse' presumably because the 'p' is more common than the 'v')
