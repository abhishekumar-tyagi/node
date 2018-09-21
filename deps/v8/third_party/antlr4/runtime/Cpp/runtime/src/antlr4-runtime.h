/* Copyright (c) 2012-2017 The ANTLR Project. All rights reserved.
 * Use of this file is governed by the BSD 3-clause license that
 * can be found in the LICENSE.txt file in the project root.
 */

#pragma once

// This is the umbrella header for all ANTLR4 C++ runtime headers.

#include "antlr4-common.h"

#include "ANTLRErrorListener.h"
#include "ANTLRErrorStrategy.h"
#include "ANTLRFileStream.h"
#include "ANTLRInputStream.h"
#include "BailErrorStrategy.h"
#include "BaseErrorListener.h"
#include "BufferedTokenStream.h"
#include "CharStream.h"
#include "CommonToken.h"
#include "CommonTokenFactory.h"
#include "CommonTokenStream.h"
#include "ConsoleErrorListener.h"
#include "DefaultErrorStrategy.h"
#include "DiagnosticErrorListener.h"
#include "Exceptions.h"
#include "FailedPredicateException.h"
#include "InputMismatchException.h"
#include "IntStream.h"
#include "InterpreterRuleContext.h"
#include "Lexer.h"
#include "LexerInterpreter.h"
#include "LexerNoViableAltException.h"
#include "ListTokenSource.h"
#include "NoViableAltException.h"
#include "Parser.h"
#include "ParserInterpreter.h"
#include "ParserRuleContext.h"
#include "ProxyErrorListener.h"
#include "RecognitionException.h"
#include "Recognizer.h"
#include "RuleContext.h"
#include "RuleContextWithAltNum.h"
#include "RuntimeMetaData.h"
#include "Token.h"
#include "TokenFactory.h"
#include "TokenSource.h"
#include "TokenStream.h"
#include "TokenStreamRewriter.h"
#include "UnbufferedCharStream.h"
#include "UnbufferedTokenStream.h"
#include "Vocabulary.h"
#include "WritableToken.h"
#include "atn/ATN.h"
#include "atn/ATNConfig.h"
#include "atn/ATNConfigSet.h"
#include "atn/ATNDeserializationOptions.h"
#include "atn/ATNDeserializer.h"
#include "atn/ATNSerializer.h"
#include "atn/ATNSimulator.h"
#include "atn/ATNState.h"
#include "atn/ATNType.h"
#include "atn/AbstractPredicateTransition.h"
#include "atn/ActionTransition.h"
#include "atn/AmbiguityInfo.h"
#include "atn/ArrayPredictionContext.h"
#include "atn/AtomTransition.h"
#include "atn/BasicBlockStartState.h"
#include "atn/BasicState.h"
#include "atn/BlockEndState.h"
#include "atn/BlockStartState.h"
#include "atn/ContextSensitivityInfo.h"
#include "atn/DecisionEventInfo.h"
#include "atn/DecisionInfo.h"
#include "atn/DecisionState.h"
#include "atn/EmptyPredictionContext.h"
#include "atn/EpsilonTransition.h"
#include "atn/ErrorInfo.h"
#include "atn/LL1Analyzer.h"
#include "atn/LexerATNConfig.h"
#include "atn/LexerATNSimulator.h"
#include "atn/LexerAction.h"
#include "atn/LexerActionExecutor.h"
#include "atn/LexerActionType.h"
#include "atn/LexerChannelAction.h"
#include "atn/LexerCustomAction.h"
#include "atn/LexerIndexedCustomAction.h"
#include "atn/LexerModeAction.h"
#include "atn/LexerMoreAction.h"
#include "atn/LexerPopModeAction.h"
#include "atn/LexerPushModeAction.h"
#include "atn/LexerSkipAction.h"
#include "atn/LexerTypeAction.h"
#include "atn/LookaheadEventInfo.h"
#include "atn/LoopEndState.h"
#include "atn/NotSetTransition.h"
#include "atn/OrderedATNConfigSet.h"
#include "atn/ParseInfo.h"
#include "atn/ParserATNSimulator.h"
#include "atn/PlusBlockStartState.h"
#include "atn/PlusLoopbackState.h"
#include "atn/PrecedencePredicateTransition.h"
#include "atn/PredicateEvalInfo.h"
#include "atn/PredicateTransition.h"
#include "atn/PredictionContext.h"
#include "atn/PredictionMode.h"
#include "atn/ProfilingATNSimulator.h"
#include "atn/RangeTransition.h"
#include "atn/RuleStartState.h"
#include "atn/RuleStopState.h"
#include "atn/RuleTransition.h"
#include "atn/SemanticContext.h"
#include "atn/SetTransition.h"
#include "atn/SingletonPredictionContext.h"
#include "atn/StarBlockStartState.h"
#include "atn/StarLoopEntryState.h"
#include "atn/StarLoopbackState.h"
#include "atn/TokensStartState.h"
#include "atn/Transition.h"
#include "atn/WildcardTransition.h"
#include "dfa/DFA.h"
#include "dfa/DFASerializer.h"
#include "dfa/DFAState.h"
#include "dfa/LexerDFASerializer.h"
#include "misc/InterpreterDataReader.h"
#include "misc/Interval.h"
#include "misc/IntervalSet.h"
#include "misc/MurmurHash.h"
#include "misc/Predicate.h"
#include "support/Any.h"
#include "support/Arrays.h"
#include "support/BitSet.h"
#include "support/CPPUtils.h"
#include "support/StringUtils.h"
#include "support/guid.h"
#include "tree/AbstractParseTreeVisitor.h"
#include "tree/ErrorNode.h"
#include "tree/ErrorNodeImpl.h"
#include "tree/ParseTree.h"
#include "tree/ParseTreeListener.h"
#include "tree/ParseTreeProperty.h"
#include "tree/ParseTreeVisitor.h"
#include "tree/ParseTreeWalker.h"
#include "tree/TerminalNode.h"
#include "tree/TerminalNodeImpl.h"
#include "tree/Trees.h"
#include "tree/pattern/Chunk.h"
#include "tree/pattern/ParseTreeMatch.h"
#include "tree/pattern/ParseTreePattern.h"
#include "tree/pattern/ParseTreePatternMatcher.h"
#include "tree/pattern/RuleTagToken.h"
#include "tree/pattern/TagChunk.h"
#include "tree/pattern/TextChunk.h"
#include "tree/pattern/TokenTagToken.h"
#include "tree/xpath/XPath.h"
#include "tree/xpath/XPathElement.h"
#include "tree/xpath/XPathLexer.h"
#include "tree/xpath/XPathLexerErrorListener.h"
#include "tree/xpath/XPathRuleAnywhereElement.h"
#include "tree/xpath/XPathRuleElement.h"
#include "tree/xpath/XPathTokenAnywhereElement.h"
#include "tree/xpath/XPathTokenElement.h"
#include "tree/xpath/XPathWildcardAnywhereElement.h"
#include "tree/xpath/XPathWildcardElement.h"
