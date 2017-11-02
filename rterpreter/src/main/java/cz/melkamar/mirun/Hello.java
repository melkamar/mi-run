/**
 * Created by Martin Melka (martin.melka@gmail.com) on 01.11.2017 22:23.
 */
package cz.melkamar.mirun;


import cz.melkamar.mirun.parser.RBaseListener;
import cz.melkamar.mirun.parser.RFilter;
import cz.melkamar.mirun.parser.RLexer;
import cz.melkamar.mirun.parser.RParser;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.ParseTree;
import org.antlr.v4.runtime.tree.ParseTreeWalker;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.Charset;
import java.nio.file.Files;

public class Hello {
//    public static void main(String[] args) {
////        try {
////            File file = new File(Hello.class.getResource("/foo.r").toURI());
//////            Parser p = new Parser();
//////            Python3Parser.File_inputContext ctx = p.parse(f);
//////            new AstPrinter().print(ctx);
////
////            String code = readFile(file, Charset.forName("UTF-8"));
////            Python3Lexer lexer = new Python3Lexer(new ANTLRInputStream(code));
////            CommonTokenStream tokens = new CommonTokenStream(lexer);
////            Python3Parser parser = new Python3Parser(tokens);
////
////            ParserRuleContext tree = parser.file_input();
////            ParseTreeWalker walker = new ParseTreeWalker();
////            MyListener listener = new MyListener(parser);
////            walker.walk(listener, tree);
////
////        } catch (URISyntaxException e) {
////            e.printStackTrace();
////        } catch (IOException e) {
////            e.printStackTrace();
////        }
//    }

    public static void main(String[] args) throws Exception {
        String inputFile = "D:\\cvut-checkouted\\mi-run\\rterpreter\\src\\main\\resources\\foo.r";
        if (args.length > 0) inputFile = args[0];
        InputStream is = System.in;
        if (inputFile != null) {
            is = new FileInputStream(inputFile);
        }
        ANTLRInputStream input = new ANTLRInputStream(is);
        RLexer lexer = new RLexer(input);
        CommonTokenStream tokens = new CommonTokenStream(lexer);
        // Print tokens BEFORE filtering
//		tokens.fill();
//		for (Object tok : tokens.getTokens()) {
//			System.out.println(tok);
//		}
        RFilter filter = new RFilter(tokens);
        filter.stream(); // call start rule: stream
        tokens.reset();
        // Print tokens AFTER filtering
//		for (Object tok : tokens.getTokens()) {
//			System.out.println(tok);
//		}
        RParser parser = new RParser(tokens);

//        ParserRuleContext tree = parser.file_input();
//        ParseTreeWalker walker = new ParseTreeWalker();
//        MyListener listener = new MyListener(parser);
//        walker.walk(listener, tree);

        parser.setBuildParseTree(true);
        RuleContext tree = parser.prog();
        //tree.save(parser, "/tmp/R.ps"); // Generate postscript
        System.out.println(tree.toStringTree(parser));


        walkChildren(tree, 0);
//        int cnt = tree.getChildCount();
//        for (int i=0; i<cnt; i++){
//            ParseTree child = tree.getChild(i);
//            System.out.println(child);
//        }
    }

    static void walkChildren(ParseTree rootNode, int depth) {
        StringBuilder indentStr = new StringBuilder();
        for (int i = 0; i < depth; i++) indentStr.append("|    ");

//        System.out.println(indentStr+rootNode.getText());

        if (rootNode instanceof RuleContext){
            RuleContext rule = (RuleContext) rootNode;
            System.out.println(indentStr+RParser.ruleNames[rule.getRuleIndex()]);

            for (int i = 0; i < rootNode.getChildCount(); i++) {
                walkChildren(rootNode.getChild(i), depth + 1);
            }
        } else {
            System.out.println(indentStr+"["+rootNode.getText()+"]");
        }
    }

//    static class RListener extends RBaseListener{
//        RParser parser = null;
//
//        public RListener(RParser parser) {
//            this.parser = parser;
//        }
//
//
//    }

    private static String readFile(File file, Charset encoding) throws IOException {
        byte[] encoded = Files.readAllBytes(file.toPath());
        return new String(encoded, encoding);
    }

//    static class MyListener extends Python3BaseListener {
//        Python3Parser parser = null;
//
//        public MyListener(Python3Parser parser) {
//            this.parser = parser;
//        }
//
//        @Override
//        public void enterFuncdef(Python3Parser.FuncdefContext ctx) {
//            TokenStream tokens = parser.getTokenStream();
//            String name = ctx.NAME().getText();
//            String args = tokens.getText(ctx.parameters());
//            System.out.println("Function definition [" + name + "   " + args+"]");
//        }
//
//        @Override
//        public void exitFuncdef(Python3Parser.FuncdefContext ctx) {
//            System.out.println("Function exit [" + ctx.NAME().getSymbol().getText()+"]");
//        }
//
//        @Override
//        public void enterFile_input(Python3Parser.File_inputContext ctx) {
//            System.out.println("file input");
//        }
//
//        @Override
//        public void enterStmt(Python3Parser.StmtContext ctx) {
//
////            TokenStream tokens = parser.getTokenStream();
////            System.out.println("STMT["+tokens.getText(ctx)+"]/STMT");
//
////            String txt = "unknown stmt";
////            System.out.println("rulen "+ctx.getRuleIndex() );
////            if (ctx.compound_stmt() != null) {
////                txt = ctx.compound_stmt().getRuleIndex() + "";
////            } else if (ctx.simple_stmt() != null) {
////                txt = ctx.simple_stmt().getRuleIndex() + "";
////            }
////            System.out.println("Enter stmt " + txt);
//        }
//
//        @Override
//        public void enterParameters(Python3Parser.ParametersContext ctx) {
//            System.out.println("Parameters: ["+ctx.getText()+"]");
//        }
//    }
}

