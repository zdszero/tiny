# TINY

TINY, a very very simple programming language according to the book 《complier constrction principles and practice》

## scanning

| Reserverd Words | Special Symbols | Other                           |
| :-:             | :-:             | :-:                             |
| if              | +               | number( 1 or more digits )      |
| then            | -               |                                 |
| else            | *               |                                 |
| end             | /               |                                 |
| repeat          | =               |                                 |
| until           | <               | identifier( 1 or more letters ) |
| read            | (               |                                 |
| write           | )               |                                 |
|                 | ;               |                                 |
|                 | :=              |                                 |


## parsing

- grammar in BNF (context-free grammar)

> + stmt-seq → stmt { ; stmt }
> + stmt → if-stmt | repeat-stmt | assign-stmt | read-stmt | write-stmt
> + if-stmt → **if** exp **then** stmt-seq [ **else** stmt-seq ] **end**
> + repeat-stmt → **repeat** stmt-seq **until** exp
> + assign-stmt → **id** **:=** exp
> + read-stmt → **read** **id**
> + write-stmt → **write** **id**
> + exp → simple-exp | simple-exp comparision-op simple-exp
> + simple-exp → simple-exp addop term | term
> + term → term mulop factor | factor
> + factor → **(** exp **)** | **number** | **id**
> + comparison-op → **<** | **=**
> + addop → **+** | **-**
> + mulop → **\***

- grammar in EBNF

> + stmt-seq → stmt { ; stmt }
> + stmt → if-stmt | repeat-stmt | assign-stmt | read-stmt | write-stmt
> + if-stmt → **if** exp **then** stmt-seq [ **else** stmt-seq ] **end**
> + repeat-stmt → **repeat** stmt-seq **until** exp
> + assign-stmt → **id** **:=** exp
> + read-stmt → **read** **id**
> + write-stmt → **write** **id**
> + exp → simple-exp [ comparision-op simple-exp ]
> + simple-exp → term { addop term }
> + term → factor { mulop factor }
> + factor → **(** exp **)** | **number** | **id**
> + comparison-op → **<** | **=**
> + addop → **+** | **-**
> + mulop → **\***

- syntax tree

node kind

1. StmtK

IfK, RepeatK, AssignK, ReadK, WriteK

2. ExpK

ConstK, OpK, IdK

