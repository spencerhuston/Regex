# Regex
C++-based regular expression matcher (personal project)

To run (single quotes are essential around the arguments):

    ./regex '[regular expression]' '[string to match]' '[2nd string]' ...

Support:

    Operations: 
        - *     zero or more
        - +     one or more
        - ?     zero or one
        - |     or 
        - [ - ] ranged or: [a-c] = (a|b|c) 
        - \     escape characters
        - { , } ranged repetition: {1,3} = aa?a?, does NOT nest
        - ( )   group: (abc)* = zero or more 'abc'
        - concatenation
        
    Other: 
        
        - variadic argument list
        - -h or --help to display usage/characters
        - Special escaped characters:
            - \d: digits
            - \w: lowercase letters
            - \W: uppercase letters 
            - \w: alpanumeric

        - To be added:
            - ^ and $ to match patterns at beginning/end of given string
            - Escaping reserved characters (\?, \+, etc.)
            - Whitespaces
            - Pattern match on files

Precedence:

    +, then * and ?, then . (concatenation), then any regular character
    
    NOTE: + overrides the 'zero or...' in * and ? so (a?)+ or (a*)+ fail to match on an empty string

Resources:

  - https://swtch.com/~rsc/regexp/regexp1.html, most of my ideas came from here regarding building the NFA (evident in Regex::parse)
        
  - https://gist.github.com/gmenard/6161825 for Shunting-yard algorithm implementation
