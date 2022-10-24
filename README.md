# wordlebot
Command line machine player of Wordle ... The program guesses words and, currently, a human types in the results Wordle generates as a string of '-','y', and 'g' characters -- meaning gray, yellow, and green respectively.

This code is C++17 using range-v3 ranges, as a Visual Studio solution. If there is interest I can turn it into a CMake project and I'd also like to switch it to use C++23 ranges eventually (but currently just putting this here to use GitHub as source control and making it public, because, why not?)

The currently it works as follows:

  1. Wordlebot has a list of five letter words along with their 'score' and their frequency. 
  
    - score  
      Given 'guessed'  and 'hidden' we consider the score of a wordle play as two times the number of 
      green tiles yielded by playing guessed against hidden plus the number of yellow tiles. The score 
      assigned to each word, then, is the normalized expected value of this score for a given guessed 
      word if the hidden word is selected by a uniformly distributed random variable. I precomputed 
      these number in code not included here. 
      
    - Frequency  
      is the normalized counts of the word appearances in some corpus. I found these counts 
      "unigram_freq.csv" on Kaggle, but may end up using some other source.
      
  2. Wordlebot's initial guess is one of the top n words ranked by score, where n is currently 10.
  
  3. After the initial guess, Wordlebot continues guessing by randomly selecting one of the top n words ranked by score that are valid given the game state, 
     where n is now 3, until the number of possible hidden words is fewer than a threshold, currently 13.
  
  4. It then begins selecting from the top n words ranked by frequency 
     that are valid given the game state

Sample session below, where the hidden word is 'stray'.

```
wordlebot guesses 'saned'
  => gy---

wordlebot guesses 'stria'
  => ggg-y

wordlebot guesses 'stray'
  => ggggg

  3/6
```

In the above the transition between guessing by score vs. by frequency happened after it played 'stria'.
