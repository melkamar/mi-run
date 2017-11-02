# rterpreter

### Installation

#### Requirements

- Gradle
- JDK

#### IntelliJ Idea:

- import this folder as module/project
- Sync gradle and all that
- Run generateGrammarSource, that will create some sources in build/generated-src/antlr/main/
- IntelliJ will not pick up those files correctly - go to File > Project Structure.
    - Choose Modules in the left hand side menu, there your created module
    - Add Content Root - enter the path to the generated files (if it is not already there).
    - Once you have done that, click on the little P icon on the right hand side, put `cz.melkamar.mirun.parser` as package prefix and check the `For generated sources` checkbox.