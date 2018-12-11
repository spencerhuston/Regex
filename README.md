# Regex
C++-based regular expression matcher (personal project)

Character support:
  *, +, ?, |, \[ - ], \\, ( ) with nesting, and concatenation

Scanner done, parser in progress

All (roughly) working, current issues:

1.) OR'd groups do not work, ex: (ab)|(cd) results in a fail for 'ab' but match for 'cd'. Cannot currently match end state of subexpression _ab_ to end state of subexpression _cd_.

2.) Anything followed by ?, +, or * and then itself does not properly work, ex: a?a fails for 'a' but matches on 'aa'. NFA cannot tell the difference between first state resulting from _a?_ or second concatenation state from _a_.
