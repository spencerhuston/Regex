# Regex
C++-based regular expression matcher (personal project)

To run (single quotes are essential around the arguments):

    ./regex '[regular expression]' '[string to match]' '[2nd string]' ...

Support:

    Characters: *, +, ?, |, [ - ], \, ( ) with nesting, and concatenation
    Other: 
        
        - variadic argument list
        
        - -h or --help to display usage/characters
        
        - Special escaped characters (working):
    
            - \d: all digits
    
            - \w: all lowercase letters
    
            - \W: all uppercase letters

        - To be added:

            - Ranged repetition: _a{x,y}_ for repeating 'a' at least x times up to y times
        
            - \a: any character
    
            - \D: all non-digits
    
            - whitespace characters (\s for space, \t for tab, etc.)
    
            - Escaping reserved characters (\?, \+, etc.)

Precedence:

    _+_, then _*_ and _?_, then _._ (concatenation), then any regular character
    
    NOTE: _+_ overrides the 'zero or...' in _*_ and _?_ so _(a?)+_ or _(a*)+_ fail to match on an empty string

Resources:

  - https://swtch.com/~rsc/regexp/regexp1.html, most of my ideas came from here regarding building the NFA (evident in Regex::parse)
        
  - https://gist.github.com/gmenard/6161825 for Shunting-yard algorithm implementation
