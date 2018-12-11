# Regex
C++-based regular expression matcher (personal project)

To run (single quotes are essential around the arguments):

    ./regex '[regular expression]' '[string to match]' '[2nd string]' ...

Support

    Characters: *, +, ?, |, \[ - ], \\, ( ) with nesting, and concatenation
    Other: variadic argument list, -h or --help to display usage/characters

Scanner done, parser in progress

All (except grouping) working mostly as intended, current issues:

1.) OR'd groups do not work, ex: (ab)|(cd) results in a fail for 'ab' but match for 'cd'. Cannot currently match end state of subexpression _ab_ to end state of subexpression _cd_.

2.) + and * following a group do not work as intended, ex: (ab)+ matches on 'ab' but fails on 'abab'; for (ab)* it fails on 'ab' but matches on 'a'

3.) Anything followed by ?, +, or * and then itself does not properly work, ex: a?a fails for 'a' but matches on 'aa'. NFA cannot tell the difference between first state resulting from _a?_ or second concatenation state from _a_.
