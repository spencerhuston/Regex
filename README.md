# Regex
C++-based regular expression matcher (personal project)

To run (single quotes are essential around the arguments):

    ./regex '[regular expression]' '[string to match]' '[2nd string]' ...

Support:

    Characters: 
        * 
        + 
        ? 
        | 
        [ - ] 
        \ 
        { , } (does NOT nest)
        ( ) with nesting
        concatenation
        
    Other: 
        
        - variadic argument list
        
        - -h or --help to display usage/characters
        
        - Special escaped characters:
    
            - \d: digits
    
            - \w: lowercase letters
    
            - \W: uppercase letters
            
            - \s: whitespace
           
            - \S: visible characters
            
            - Whitespace characters: \n, \t, \r, \v, and \f

        - To be added:
        
            - ^ and $ to match patterns at beginning/end of given string
    
            - Escaping reserved characters (\?, \+, etc.)

Precedence:

    +, then * and ?, then . (concatenation), then any regular character
    
    NOTE: + overrides the 'zero or...' in * and ? so (a?)+ or (a*)+ fail to match on an empty string

Resources:

  - https://swtch.com/~rsc/regexp/regexp1.html, most of my ideas came from here regarding building the NFA (evident in Regex::parse)
        
  - https://gist.github.com/gmenard/6161825 for Shunting-yard algorithm implementation
