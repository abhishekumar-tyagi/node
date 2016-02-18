/**
 * @fileoverview A class to manage state of generating a code path.
 * @author Toru Nagashima
 * @copyright 2015 Toru Nagashima. All rights reserved.
 * See LICENSE file in root directory for full license.
 */

"use strict";

//------------------------------------------------------------------------------
// Requirements
//------------------------------------------------------------------------------

var CodePathSegment = require("./code-path-segment"),
    ForkContext = require("./fork-context");

//------------------------------------------------------------------------------
// Helpers
//------------------------------------------------------------------------------

/**
 * Adds given segments into the `dest` array.
 * If the `others` array does not includes the given segments, adds to the `all`
 * array as well.
 *
 * This adds only reachable and used segments.
 *
 * @param {CodePathSegment[]} dest - A destination array (`returnedSegments` or `thrownSegments`).
 * @param {CodePathSegment[]} others - Another destination array (`returnedSegments` or `thrownSegments`).
 * @param {CodePathSegment[]} all - The unified destination array (`finalSegments`).
 * @param {CodePathSegment[]} segments - Segments to add.
 * @returns {void}
 */
function addToReturnedOrThrown(dest, others, all, segments) {
    for (var i = 0; i < segments.length; ++i) {
        var segment = segments[i];

        dest.push(segment);
        if (others.indexOf(segment) === -1) {
            all.push(segment);
        }
    }
}

/**
 * Gets a loop-context for a `continue` statement.
 *
 * @param {CodePathState} state - A state to get.
 * @param {string} label - The label of a `continue` statement.
 * @returns {LoopContext} A loop-context for a `continue` statement.
 */
function getContinueContext(state, label) {
    if (!label) {
        return state.loopContext;
    }

    var context = state.loopContext;
    while (context) {
        if (context.label === label) {
            return context;
        }
        context = context.upper;
    }

    /* istanbul ignore next: foolproof (syntax error) */
    return null;
}

/**
 * Gets a context for a `break` statement.
 *
 * @param {CodePathState} state - A state to get.
 * @param {string} label - The label of a `break` statement.
 * @returns {LoopContext|SwitchContext} A context for a `break` statement.
 */
function getBreakContext(state, label) {
    var context = state.breakContext;
    while (context) {
        if (label ? context.label === label : context.breakable) {
            return context;
        }
        context = context.upper;
    }

    /* istanbul ignore next: foolproof (syntax error) */
    return null;
}

/**
 * Gets a context for a `return` statement.
 *
 * @param {CodePathState} state - A state to get.
 * @returns {TryContext|CodePathState} A context for a `return` statement.
 */
function getReturnContext(state) {
    var context = state.tryContext;
    while (context) {
        if (context.hasFinalizer && context.position !== "finally") {
            return context;
        }
        context = context.upper;
    }

    return state;
}

/**
 * Gets a context for a `throw` statement.
 *
 * @param {CodePathState} state - A state to get.
 * @returns {TryContext|CodePathState} A context for a `throw` statement.
 */
function getThrowContext(state) {
    var context = state.tryContext;
    while (context) {
        if (context.position === "try" ||
            (context.hasFinalizer && context.position === "catch")
        ) {
            return context;
        }
        context = context.upper;
    }

    return state;
}

/**
 * Removes a given element from a given array.
 *
 * @param {any[]} xs - An array to remove the specific element.
 * @param {any} x - An element to be removed.
 * @returns {void}
 */
function remove(xs, x) {
    xs.splice(xs.indexOf(x), 1);
}

/**
 * Disconnect given segments.
 *
 * This is used in a process for switch statements.
 * If there is the "default" chunk before other cases, the order is different
 * between node's and running's.
 *
 * @param {CodePathSegment[]} prevSegments - Forward segments to disconnect.
 * @param {CodePathSegment[]} nextSegments - Backward segments to disconnect.
 * @returns {void}
 */
function removeConnection(prevSegments, nextSegments) {
    for (var i = 0; i < prevSegments.length; ++i) {
        var prevSegment = prevSegments[i];
        var nextSegment = nextSegments[i];

        remove(prevSegment.nextSegments, nextSegment);
        remove(prevSegment.allNextSegments, nextSegment);
        remove(nextSegment.prevSegments, prevSegment);
        remove(nextSegment.allPrevSegments, prevSegment);
    }
}

/**
 * Creates looping path.
 *
 * @param {CodePathState} state - The instance.
 * @param {CodePathSegment[]} fromSegments - Segments which are source.
 * @param {CodePathSegment[]} toSegments - Segments which are destination.
 * @returns {void}
 */
function makeLooped(state, fromSegments, toSegments) {
    var end = Math.min(fromSegments.length, toSegments.length);
    for (var i = 0; i < end; ++i) {
        var fromSegment = fromSegments[i];
        var toSegment = toSegments[i];

        if (toSegment.reachable) {
            fromSegment.nextSegments.push(toSegment);
        }
        if (fromSegment.reachable) {
            toSegment.prevSegments.push(fromSegment);
        }
        fromSegment.allNextSegments.push(toSegment);
        toSegment.allPrevSegments.push(fromSegment);

        state.notifyLooped(fromSegment, toSegment);
    }
}

/**
 * Finalizes segments of `test` chunk of a ForStatement.
 *
 * - Adds `false` paths to paths which are leaving from the loop.
 * - Sets `true` paths to paths which go to the body.
 *
 * @param {LoopContext} context - A loop context to modify.
 * @param {ChoiceContext} choiceContext - A choice context of this loop.
 * @param {CodePathSegment[]} head - The current head paths.
 * @returns {void}
 */
function finalizeTestSegmentsOfFor(context, choiceContext, head) {
    if (!choiceContext.processed) {
        choiceContext.trueForkContext.add(head);
        choiceContext.falseForkContext.add(head);
    }

    if (context.test !== true) {
        context.brokenForkContext.addAll(choiceContext.falseForkContext);
    }
    context.endOfTestSegments = choiceContext.trueForkContext.makeNext(0, -1);
}

//------------------------------------------------------------------------------
// Public Interface
//------------------------------------------------------------------------------

/**
 * A class which manages state to analyze code paths.
 *
 * @constructor
 * @param {IdGenerator} idGenerator - An id generator to generate id for code
 *   path segments.
 * @param {function} onLooped - A callback function to notify looping.
 */
function CodePathState(idGenerator, onLooped) {
    this.idGenerator = idGenerator;
    this.notifyLooped = onLooped;
    this.forkContext = ForkContext.newRoot(idGenerator);
    this.choiceContext = null;
    this.switchContext = null;
    this.tryContext = null;
    this.loopContext = null;
    this.breakContext = null;

    this.currentSegments = [];
    this.initialSegment = this.forkContext.head[0];

    // returnedSegments and thrownSegments push elements into finalSegments also.
    var final = this.finalSegments = [];
    var returned = this.returnedForkContext = [];
    var thrown = this.thrownForkContext = [];
    returned.add = addToReturnedOrThrown.bind(null, returned, thrown, final);
    thrown.add = addToReturnedOrThrown.bind(null, thrown, returned, final);
}

CodePathState.prototype = {
    constructor: CodePathState,

    /**
     * The head segments.
     * @type {CodePathSegment[]}
     */
    get headSegments() {
        return this.forkContext.head;
    },

    /**
     * The parent forking context.
     * This is used for the root of new forks.
     * @type {ForkContext}
     */
    get parentForkContext() {
        var current = this.forkContext;
        return current && current.upper;
    },

    /**
     * Creates and stacks new forking context.
     *
     * @param {boolean} forkLeavingPath - A flag which shows being in a
     *   "finally" block.
     * @returns {ForkContext} The created context.
     */
    pushForkContext: function(forkLeavingPath) {
        this.forkContext = ForkContext.newEmpty(
            this.forkContext,
            forkLeavingPath
        );

        return this.forkContext;
    },

    /**
     * Pops and merges the last forking context.
     * @returns {ForkContext} The last context.
     */
    popForkContext: function() {
        var lastContext = this.forkContext;

        this.forkContext = lastContext.upper;
        this.forkContext.replaceHead(lastContext.makeNext(0, -1));

        return lastContext;
    },

    /**
     * Creates a new path.
     * @returns {void}
     */
    forkPath: function() {
        this.forkContext.add(this.parentForkContext.makeNext(-1, -1));
    },

    /**
     * Creates a bypass path.
     * This is used for such as IfStatement which does not have "else" chunk.
     *
     * @returns {void}
     */
    forkBypassPath: function() {
        this.forkContext.add(this.parentForkContext.head);
    },

    //--------------------------------------------------------------------------
    // ConditionalExpression, LogicalExpression, IfStatement
    //--------------------------------------------------------------------------

    /**
     * Creates a context for ConditionalExpression, LogicalExpression,
     * IfStatement, WhileStatement, DoWhileStatement, or ForStatement.
     *
     * LogicalExpressions have cases that it goes different paths between the
     * `true` case and the `false` case.
     *
     * For Example:
     *
     *     if (a || b) {
     *         foo();
     *     } else {
     *         bar();
     *     }
     *
     * In this case, `b` is evaluated always in the code path of the `else`
     * block, but it's not so in the code path of the `if` block.
     * So there are 3 paths.
     *
     *     a -> foo();
     *     a -> b -> foo();
     *     a -> b -> bar();
     *
     * @param {string} kind - A kind string.
     *   If the new context is LogicalExpression's, this is `"&&"` or `"||"`.
     *   If it's IfStatement's or ConditionalExpression's, this is `"test"`.
     *   Otherwise, this is `"loop"`.
     * @param {boolean} isForkingAsResult - A flag that shows that goes different
     *   paths between `true` and `false`.
     * @returns {void}
     */
    pushChoiceContext: function(kind, isForkingAsResult) {
        this.choiceContext = {
            upper: this.choiceContext,
            kind: kind,
            isForkingAsResult: isForkingAsResult,
            trueForkContext: ForkContext.newEmpty(this.forkContext),
            falseForkContext: ForkContext.newEmpty(this.forkContext),
            processed: false
        };
    },

    /**
     * Pops the last choice context and finalizes it.
     *
     * @returns {ChoiceContext} The popped context.
     */
    popChoiceContext: function() {
        var context = this.choiceContext;
        this.choiceContext = context.upper;

        var forkContext = this.forkContext;
        var headSegments = forkContext.head;

        switch (context.kind) {
            case "&&":
            case "||":
                // If any result were not transferred from child contexts,
                // this sets the head segments to both cases.
                // The head segments are the path of the right-hand operand.
                if (!context.processed) {
                    context.trueForkContext.add(headSegments);
                    context.falseForkContext.add(headSegments);
                }

                // Transfers results to upper context if this context is in
                // test chunk.
                if (context.isForkingAsResult) {
                    var parentContext = this.choiceContext;
                    parentContext.trueForkContext.addAll(context.trueForkContext);
                    parentContext.falseForkContext.addAll(context.falseForkContext);
                    parentContext.processed = true;

                    return context;
                }
                break;

            case "test":
                if (!context.processed) {
                    // The head segments are the path of the `if` block here.
                    // Updates the `true` path with the end of the `if` block.
                    context.trueForkContext.clear();
                    context.trueForkContext.add(headSegments);
                } else {
                    // The head segments are the path of the `else` block here.
                    // Updates the `false` path with the end of the `else` block.
                    context.falseForkContext.clear();
                    context.falseForkContext.add(headSegments);
                }
                break;

            case "loop":
                // Loops are addressed in popLoopContext().
                // This is called from popLoopContext().
                return context;

            /* istanbul ignore next */
            default:
                throw new Error("unreachable");
        }

        // Merges all paths.
        var prevForkContext = context.trueForkContext;
        prevForkContext.addAll(context.falseForkContext);
        forkContext.replaceHead(prevForkContext.makeNext(0, -1));

        return context;
    },

    /**
     * Makes a code path segment of the right-hand operand of a logical
     * expression.
     *
     * @returns {void}
     */
    makeLogicalRight: function() {
        var context = this.choiceContext;
        var forkContext = this.forkContext;

        if (context.processed) {
            // This got segments already from the child choice context.
            // Creates the next path from own true/false fork context.
            var prevForkContext =
                context.kind === "&&" ? context.trueForkContext :
                /* kind === "||" */ context.falseForkContext;

            forkContext.replaceHead(prevForkContext.makeNext(0, -1));
            prevForkContext.clear();

            context.processed = false;
        } else {
            // This did not get segments from the child choice context.
            // So addresses the head segments.
            // The head segments are the path of the left-hand operand.
            if (context.kind === "&&") {
                // The path does short-circuit if false.
                context.falseForkContext.add(forkContext.head);
            } else {
                // The path does short-circuit if true.
                context.trueForkContext.add(forkContext.head);
            }

            forkContext.replaceHead(forkContext.makeNext(-1, -1));
        }
    },

    /**
     * Makes a code path segment of the `if` block.
     *
     * @returns {void}
     */
    makeIfConsequent: function() {
        var context = this.choiceContext;
        var forkContext = this.forkContext;

        // If any result were not transferred from child contexts,
        // this sets the head segments to both cases.
        // The head segments are the path of the test expression.
        if (!context.processed) {
            context.trueForkContext.add(forkContext.head);
            context.falseForkContext.add(forkContext.head);
        }
        context.processed = false;

        // Creates new path from the `true` case.
        forkContext.replaceHead(
            context.trueForkContext.makeNext(0, -1)
        );
    },

    /**
     * Makes a code path segment of the `else` block.
     *
     * @returns {void}
     */
    makeIfAlternate: function() {
        var context = this.choiceContext;
        var forkContext = this.forkContext;

        // The head segments are the path of the `if` block.
        // Updates the `true` path with the end of the `if` block.
        context.trueForkContext.clear();
        context.trueForkContext.add(forkContext.head);
        context.processed = true;

        // Creates new path from the `false` case.
        forkContext.replaceHead(
            context.falseForkContext.makeNext(0, -1)
        );
    },

    //--------------------------------------------------------------------------
    // SwitchStatement
    //--------------------------------------------------------------------------

    /**
     * Creates a context object of SwitchStatement and stacks it.
     *
     * @param {boolean} hasCase - `true` if the switch statement has one or more
     *   case parts.
     * @param {string|null} label - The label text.
     * @returns {void}
     */
    pushSwitchContext: function(hasCase, label) {
        this.switchContext = {
            upper: this.switchContext,
            hasCase: hasCase,
            defaultSegments: null,
            defaultBodySegments: null,
            foundDefault: false,
            lastIsDefault: false,
            countForks: 0
        };
        this.pushBreakContext(true, label);
    },

    /**
     * Pops the last context of SwitchStatement and finalizes it.
     *
     * - Disposes all forking stack for `case` and `default`.
     * - Creates the next code path segment from `context.brokenForkContext`.
     * - If the last `SwitchCase` node is not a `default` part, creates a path
     *   to the `default` body.
     *
     * @returns {void}
     */
    popSwitchContext: function() {
        var context = this.switchContext;
        this.switchContext = context.upper;

        var forkContext = this.forkContext;
        var brokenForkContext = this.popBreakContext().brokenForkContext;

        if (context.countForks === 0) {
            // When there is only one `default` chunk and there is one or more
            // `break` statements, even if forks are nothing, it needs to merge
            // those.
            if (!brokenForkContext.empty) {
                brokenForkContext.add(forkContext.makeNext(-1, -1));
                forkContext.replaceHead(brokenForkContext.makeNext(0, -1));
            }
            return;
        }

        var lastSegments = forkContext.head;
        this.forkBypassPath();
        var lastCaseSegments = forkContext.head;

        // `brokenForkContext` is used to make the next segment.
        // It must add the last segment into `brokenForkContext`.
        brokenForkContext.add(lastSegments);

        // A path which is failed in all case test should be connected to path
        // of `default` chunk.
        if (!context.lastIsDefault) {
            if (context.defaultBodySegments) {
                // Remove a link from `default` label to its chunk.
                // It's false route.
                removeConnection(context.defaultSegments, context.defaultBodySegments);
                makeLooped(this, lastCaseSegments, context.defaultBodySegments);
            } else {
                // It handles the last case body as broken if `default` chunk
                // does not exist.
                brokenForkContext.add(lastCaseSegments);
            }
        }

        // Pops the segment context stack until the entry segment.
        for (var i = 0; i < context.countForks; ++i) {
            this.forkContext = this.forkContext.upper;
        }
        // Creates a path from all brokenForkContext paths.
        // This is a path after switch statement.
        this.forkContext.replaceHead(brokenForkContext.makeNext(0, -1));
    },

    /**
     * Makes a code path segment for a `SwitchCase` node.
     *
     * @param {boolean} isEmpty - `true` if the body is empty.
     * @param {boolean} isDefault - `true` if the body is the default case.
     * @returns {void}
     */
    makeSwitchCaseBody: function(isEmpty, isDefault) {
        var context = this.switchContext;
        if (!context.hasCase) {
            return;
        }

        // Merge forks.
        // The parent fork context has two segments.
        // Those are from the current case and the body of the previous case.
        var parentForkContext = this.forkContext;
        var forkContext = this.pushForkContext();
        forkContext.add(parentForkContext.makeNext(0, -1));

        // Save `default` chunk info.
        // If the `default` label is not at the last, we must make a path from
        // the last `case` to the `default` chunk.
        if (isDefault) {
            context.defaultSegments = parentForkContext.head;
            if (isEmpty) {
                context.foundDefault = true;
            } else {
                context.defaultBodySegments = forkContext.head;
            }
        } else {
            if (!isEmpty && context.foundDefault) {
                context.foundDefault = false;
                context.defaultBodySegments = forkContext.head;
            }
        }
        context.lastIsDefault = isDefault;
        context.countForks += 1;
    },

    //--------------------------------------------------------------------------
    // TryStatement
    //--------------------------------------------------------------------------

    /**
     * Creates a context object of TryStatement and stacks it.
     *
     * @param {boolean} hasFinalizer - `true` if the try statement has a
     *   `finally` block.
     * @returns {void}
     */
    pushTryContext: function(hasFinalizer) {
        this.tryContext = {
            upper: this.tryContext,
            position: "try",
            hasFinalizer: hasFinalizer,
            returnedForkContext: hasFinalizer
                ? ForkContext.newEmpty(this.forkContext)
                : null,
            thrownForkContext: ForkContext.newEmpty(this.forkContext),
            lastOfTryIsReachable: false,
            lastOfCatchIsReachable: false
        };
    },

    /**
     * Pops the last context of TryStatement and finalizes it.
     *
     * @returns {void}
     */
    popTryContext: function() {
        var context = this.tryContext;
        this.tryContext = context.upper;

        if (context.position === "catch") {
            // Merges two paths from the `try` block and `catch` block merely.
            this.popForkContext();
            return;
        }
        // The following process is executed only when there is the `finally`
        // block.

        var returned = context.returnedForkContext;
        var thrown = context.thrownForkContext;
        if (returned.empty && thrown.empty) {
            return;
        }

        // Separate head to normal paths and leaving paths.
        var headSegments = this.forkContext.head;
        this.forkContext = this.forkContext.upper;
        var normalSegments = headSegments.slice(0, headSegments.length / 2 | 0);
        var leavingSegments = headSegments.slice(headSegments.length / 2 | 0);

        // Forwards the leaving path to upper contexts.
        if (!returned.empty) {
            getReturnContext(this).returnedForkContext.add(leavingSegments);
        }
        if (!thrown.empty) {
            getThrowContext(this).thrownForkContext.add(leavingSegments);
        }

        // Sets the normal path as the next.
        this.forkContext.replaceHead(normalSegments);

        // If both paths of the `try` block and the `catch` block are
        // unreachable, the next path becomes unreachable as well.
        if (!context.lastOfTryIsReachable && !context.lastOfCatchIsReachable) {
            this.forkContext.makeUnreachable();
        }
    },

    /**
     * Makes a code path segment for a `catch` block.
     *
     * @returns {void}
     */
    makeCatchBlock: function() {
        var context = this.tryContext;
        var forkContext = this.forkContext;
        var thrown = context.thrownForkContext;

        // Update state.
        context.position = "catch";
        context.thrownForkContext = ForkContext.newEmpty(forkContext);
        context.lastOfTryIsReachable = forkContext.reachable;

        // Merge thrown paths.
        thrown.add(forkContext.head);
        var thrownSegments = thrown.makeNext(0, -1);

        // Fork to a bypass and the merged thrown path.
        this.pushForkContext();
        this.forkBypassPath();
        this.forkContext.add(thrownSegments);
    },

    /**
     * Makes a code path segment for a `finally` block.
     *
     * In the `finally` block, parallel paths are created. The parallel paths
     * are used as leaving-paths. The leaving-paths are paths from `return`
     * statements and `throw` statements in a `try` block or a `catch` block.
     *
     * @returns {void}
     */
    makeFinallyBlock: function() {
        var context = this.tryContext;
        var forkContext = this.forkContext;
        var returned = context.returnedForkContext;
        var thrown = context.thrownForkContext;
        var headOfLeavingSegments = forkContext.head;

        // Update state.
        if (context.position === "catch") {
            // Merges two paths from the `try` block and `catch` block.
            this.popForkContext();
            forkContext = this.forkContext;

            context.lastOfCatchIsReachable = forkContext.reachable;
        } else {
            context.lastOfTryIsReachable = forkContext.reachable;
        }
        context.position = "finally";

        if (returned.empty && thrown.empty) {
            // This path does not leave.
            return;
        }

        // Create a parallel segment from merging returned and thrown.
        // This segment will leave at the end of this finally block.
        var segments = forkContext.makeNext(-1, -1);
        var j;
        for (var i = 0; i < forkContext.count; ++i) {
            var prevSegsOfLeavingSegment = [headOfLeavingSegments[i]];

            for (j = 0; j < returned.segmentsList.length; ++j) {
                prevSegsOfLeavingSegment.push(returned.segmentsList[j][i]);
            }
            for (j = 0; j < thrown.segmentsList.length; ++j) {
                prevSegsOfLeavingSegment.push(thrown.segmentsList[j][i]);
            }

            segments.push(CodePathSegment.newNext(
                this.idGenerator.next(),
                prevSegsOfLeavingSegment));
        }

        this.pushForkContext(true);
        this.forkContext.add(segments);
    },

    /**
     * Makes a code path segment from the first throwable node to the `catch`
     * block or the `finally` block.
     *
     * @returns {void}
     */
    makeFirstThrowablePathInTryBlock: function() {
        var forkContext = this.forkContext;
        if (!forkContext.reachable) {
            return;
        }

        var context = getThrowContext(this);
        if (context === this ||
            context.position !== "try" ||
            !context.thrownForkContext.empty
        ) {
            return;
        }

        context.thrownForkContext.add(forkContext.head);
        forkContext.replaceHead(forkContext.makeNext(-1, -1));
    },

    //--------------------------------------------------------------------------
    // Loop Statements
    //--------------------------------------------------------------------------

    /**
     * Creates a context object of a loop statement and stacks it.
     *
     * @param {string} type - The type of the node which was triggered. One of
     *   `WhileStatement`, `DoWhileStatement`, `ForStatement`, `ForInStatement`,
     *   and `ForStatement`.
     * @param {string|null} label - A label of the node which was triggered.
     * @returns {void}
     */
    pushLoopContext: function(type, label) {
        var forkContext = this.forkContext;
        var breakContext = this.pushBreakContext(true, label);

        switch (type) {
            case "WhileStatement":
                this.pushChoiceContext("loop", false);
                this.loopContext = {
                    upper: this.loopContext,
                    type: type,
                    label: label,
                    test: void 0,
                    continueDestSegments: null,
                    brokenForkContext: breakContext.brokenForkContext
                };
                break;

            case "DoWhileStatement":
                this.pushChoiceContext("loop", false);
                this.loopContext = {
                    upper: this.loopContext,
                    type: type,
                    label: label,
                    test: void 0,
                    entrySegments: null,
                    continueForkContext: ForkContext.newEmpty(forkContext),
                    brokenForkContext: breakContext.brokenForkContext
                };
                break;

            case "ForStatement":
                this.pushChoiceContext("loop", false);
                this.loopContext = {
                    upper: this.loopContext,
                    type: type,
                    label: label,
                    test: void 0,
                    endOfInitSegments: null,
                    testSegments: null,
                    endOfTestSegments: null,
                    updateSegments: null,
                    endOfUpdateSegments: null,
                    continueDestSegments: null,
                    brokenForkContext: breakContext.brokenForkContext
                };
                break;

            case "ForInStatement":
            case "ForOfStatement":
                this.loopContext = {
                    upper: this.loopContext,
                    type: type,
                    label: label,
                    prevSegments: null,
                    leftSegments: null,
                    endOfLeftSegments: null,
                    continueDestSegments: null,
                    brokenForkContext: breakContext.brokenForkContext
                };
                break;

            /* istanbul ignore next */
            default:
                throw new Error("unknown type: \"" + type + "\"");
        }
    },

    /**
     * Pops the last context of a loop statement and finalizes it.
     *
     * @returns {void}
     */
    popLoopContext: function() {
        var context = this.loopContext;
        this.loopContext = context.upper;

        var forkContext = this.forkContext;
        var brokenForkContext = this.popBreakContext().brokenForkContext;
        var choiceContext;

        // Creates a looped path.
        switch (context.type) {
            case "WhileStatement":
            case "ForStatement":
                choiceContext = this.popChoiceContext();
                makeLooped(
                    this,
                    forkContext.head,
                    context.continueDestSegments);
                break;

            case "DoWhileStatement":
                choiceContext = this.popChoiceContext();

                if (!choiceContext.processed) {
                    choiceContext.trueForkContext.add(forkContext.head);
                    choiceContext.falseForkContext.add(forkContext.head);
                }
                if (context.test !== true) {
                    brokenForkContext.addAll(choiceContext.falseForkContext);
                }

                // `true` paths go to looping.
                var segmentsList = choiceContext.trueForkContext.segmentsList;
                for (var i = 0; i < segmentsList.length; ++i) {
                    makeLooped(
                        this,
                        segmentsList[i],
                        context.entrySegments);
                }
                break;

            case "ForInStatement":
            case "ForOfStatement":
                brokenForkContext.add(forkContext.head);
                makeLooped(
                    this,
                    forkContext.head,
                    context.leftSegments);
                break;

            /* istanbul ignore next */
            default:
                throw new Error("unreachable");
        }

        // Go next.
        if (brokenForkContext.empty) {
            forkContext.replaceHead(forkContext.makeUnreachable(-1, -1));
        } else {
            forkContext.replaceHead(brokenForkContext.makeNext(0, -1));
        }
    },

    /**
     * Makes a code path segment for the test part of a WhileStatement.
     *
     * @param {boolean|undefined} test - The test value (only when constant).
     * @returns {void}
     */
    makeWhileTest: function(test) {
        var context = this.loopContext;
        var forkContext = this.forkContext;
        var testSegments = forkContext.makeNext(0, -1);

        // Update state.
        context.test = test;
        context.continueDestSegments = testSegments;
        forkContext.replaceHead(testSegments);
    },

    /**
     * Makes a code path segment for the body part of a WhileStatement.
     *
     * @returns {void}
     */
    makeWhileBody: function() {
        var context = this.loopContext;
        var choiceContext = this.choiceContext;
        var forkContext = this.forkContext;

        if (!choiceContext.processed) {
            choiceContext.trueForkContext.add(forkContext.head);
            choiceContext.falseForkContext.add(forkContext.head);
        }

        // Update state.
        if (context.test !== true) {
            context.brokenForkContext.addAll(choiceContext.falseForkContext);
        }
        forkContext.replaceHead(choiceContext.trueForkContext.makeNext(0, -1));
    },

    /**
     * Makes a code path segment for the body part of a DoWhileStatement.
     *
     * @returns {void}
     */
    makeDoWhileBody: function() {
        var context = this.loopContext;
        var forkContext = this.forkContext;
        var bodySegments = forkContext.makeNext(-1, -1);

        // Update state.
        context.entrySegments = bodySegments;
        forkContext.replaceHead(bodySegments);
    },

    /**
     * Makes a code path segment for the test part of a DoWhileStatement.
     *
     * @param {boolean|undefined} test - The test value (only when constant).
     * @returns {void}
     */
    makeDoWhileTest: function(test) {
        var context = this.loopContext;
        var forkContext = this.forkContext;

        context.test = test;

        // Creates paths of `continue` statements.
        if (!context.continueForkContext.empty) {
            context.continueForkContext.add(forkContext.head);
            var testSegments = context.continueForkContext.makeNext(0, -1);

            forkContext.replaceHead(testSegments);
        }
    },

    /**
     * Makes a code path segment for the test part of a ForStatement.
     *
     * @param {boolean|undefined} test - The test value (only when constant).
     * @returns {void}
     */
    makeForTest: function(test) {
        var context = this.loopContext;
        var forkContext = this.forkContext;
        var endOfInitSegments = forkContext.head;
        var testSegments = forkContext.makeNext(-1, -1);

        // Update state.
        context.test = test;
        context.endOfInitSegments = endOfInitSegments;
        context.continueDestSegments = context.testSegments = testSegments;
        forkContext.replaceHead(testSegments);
    },

    /**
     * Makes a code path segment for the update part of a ForStatement.
     *
     * @returns {void}
     */
    makeForUpdate: function() {
        var context = this.loopContext;
        var choiceContext = this.choiceContext;
        var forkContext = this.forkContext;

        // Make the next paths of the test.
        if (context.testSegments) {
            finalizeTestSegmentsOfFor(
                context,
                choiceContext,
                forkContext.head);
        } else {
            context.endOfInitSegments = forkContext.head;
        }

        // Update state.
        var updateSegments = forkContext.makeDisconnected(-1, -1);
        context.continueDestSegments = context.updateSegments = updateSegments;
        forkContext.replaceHead(updateSegments);
    },

    /**
     * Makes a code path segment for the body part of a ForStatement.
     *
     * @returns {void}
     */
    makeForBody: function() {
        var context = this.loopContext;
        var choiceContext = this.choiceContext;
        var forkContext = this.forkContext;

        // Update state.
        if (context.updateSegments) {
            context.endOfUpdateSegments = forkContext.head;

            // `update` -> `test`
            if (context.testSegments) {
                makeLooped(
                    this,
                    context.endOfUpdateSegments,
                    context.testSegments);
            }
        } else if (context.testSegments) {
            finalizeTestSegmentsOfFor(
                context,
                choiceContext,
                forkContext.head);
        } else {
            context.endOfInitSegments = forkContext.head;
        }

        var bodySegments = context.endOfTestSegments;
        if (!bodySegments) {
            // If there is not the `test` part, the `body` path comes from the
            // `init` part and the `update` part.
            var prevForkContext = ForkContext.newEmpty(forkContext);
            prevForkContext.add(context.endOfInitSegments);
            if (context.endOfUpdateSegments) {
                prevForkContext.add(context.endOfUpdateSegments);
            }

            bodySegments = prevForkContext.makeNext(0, -1);
        }
        context.continueDestSegments = context.continueDestSegments || bodySegments;
        forkContext.replaceHead(bodySegments);
    },

    /**
     * Makes a code path segment for the left part of a ForInStatement and a
     * ForOfStatement.
     *
     * @returns {void}
     */
    makeForInOfLeft: function() {
        var context = this.loopContext;
        var forkContext = this.forkContext;
        var leftSegments = forkContext.makeDisconnected(-1, -1);

        // Update state.
        context.prevSegments = forkContext.head;
        context.leftSegments = context.continueDestSegments = leftSegments;
        forkContext.replaceHead(leftSegments);
    },

    /**
     * Makes a code path segment for the right part of a ForInStatement and a
     * ForOfStatement.
     *
     * @returns {void}
     */
    makeForInOfRight: function() {
        var context = this.loopContext;
        var forkContext = this.forkContext;
        var temp = ForkContext.newEmpty(forkContext);
        temp.add(context.prevSegments);
        var rightSegments = temp.makeNext(-1, -1);

        // Update state.
        context.endOfLeftSegments = forkContext.head;
        forkContext.replaceHead(rightSegments);
    },

    /**
     * Makes a code path segment for the body part of a ForInStatement and a
     * ForOfStatement.
     *
     * @returns {void}
     */
    makeForInOfBody: function() {
        var context = this.loopContext;
        var forkContext = this.forkContext;
        var temp = ForkContext.newEmpty(forkContext);
        temp.add(context.endOfLeftSegments);
        var bodySegments = temp.makeNext(-1, -1);

        // Make a path: `right` -> `left`.
        makeLooped(this, forkContext.head, context.leftSegments);

        // Update state.
        context.brokenForkContext.add(forkContext.head);
        forkContext.replaceHead(bodySegments);
    },

    //--------------------------------------------------------------------------
    // Control Statements
    //--------------------------------------------------------------------------

    /**
     * Creates new context for BreakStatement.
     *
     * @param {boolean} breakable - The flag to indicate it can break by
     *      an unlabeled BreakStatement.
     * @param {string|null} label - The label of this context.
     * @returns {object} The new context.
     */
    pushBreakContext: function(breakable, label) {
        this.breakContext = {
            upper: this.breakContext,
            breakable: breakable,
            label: label,
            brokenForkContext: ForkContext.newEmpty(this.forkContext)
        };
        return this.breakContext;
    },

    /**
     * Removes the top item of the break context stack.
     *
     * @returns {object} The removed context.
     */
    popBreakContext: function() {
        var context = this.breakContext;
        var forkContext = this.forkContext;
        this.breakContext = context.upper;

        // Process this context here for other than switches and loops.
        if (!context.breakable) {
            var brokenForkContext = context.brokenForkContext;
            if (!brokenForkContext.empty) {
                brokenForkContext.add(forkContext.head);
                forkContext.replaceHead(brokenForkContext.makeNext(0, -1));
            }
        }

        return context;
    },

    /**
     * Makes a path for a `break` statement.
     *
     * It registers the head segment to a context of `break`.
     * It makes new unreachable segment, then it set the head with the segment.
     *
     * @param {string} label - A label of the break statement.
     * @returns {void}
     */
    makeBreak: function(label) {
        var forkContext = this.forkContext;
        if (!forkContext.reachable) {
            return;
        }

        var context = getBreakContext(this, label);
        /* istanbul ignore else: foolproof (syntax error) */
        if (context) {
            context.brokenForkContext.add(forkContext.head);
        }
        forkContext.replaceHead(forkContext.makeUnreachable(-1, -1));
    },

    /**
     * Makes a path for a `continue` statement.
     *
     * It makes a looping path.
     * It makes new unreachable segment, then it set the head with the segment.
     *
     * @param {string} label - A label of the continue statement.
     * @returns {void}
     */
    makeContinue: function(label) {
        var forkContext = this.forkContext;
        if (!forkContext.reachable) {
            return;
        }

        var context = getContinueContext(this, label);
        /* istanbul ignore else: foolproof (syntax error) */
        if (context) {
            if (context.continueDestSegments) {
                makeLooped(this, forkContext.head, context.continueDestSegments);

                // If the context is a for-in/of loop, this effects a break also.
                if (context.type === "ForInStatement" ||
                    context.type === "ForOfStatement"
                ) {
                    context.brokenForkContext.add(forkContext.head);
                }
            } else {
                context.continueForkContext.add(forkContext.head);
            }
        }
        forkContext.replaceHead(forkContext.makeUnreachable(-1, -1));
    },

    /**
     * Makes a path for a `return` statement.
     *
     * It registers the head segment to a context of `return`.
     * It makes new unreachable segment, then it set the head with the segment.
     *
     * @returns {void}
     */
    makeReturn: function() {
        var forkContext = this.forkContext;
        if (forkContext.reachable) {
            getReturnContext(this).returnedForkContext.add(forkContext.head);
            forkContext.replaceHead(forkContext.makeUnreachable(-1, -1));
        }
    },

    /**
     * Makes a path for a `throw` statement.
     *
     * It registers the head segment to a context of `throw`.
     * It makes new unreachable segment, then it set the head with the segment.
     *
     * @returns {void}
     */
    makeThrow: function() {
        var forkContext = this.forkContext;
        if (forkContext.reachable) {
            getThrowContext(this).thrownForkContext.add(forkContext.head);
            forkContext.replaceHead(forkContext.makeUnreachable(-1, -1));
        }
    },

    /**
     * Makes the final path.
     * @returns {void}
     */
    makeFinal: function() {
        var segments = this.currentSegments;
        if (segments.length > 0 && segments[0].reachable) {
            this.returnedForkContext.add(segments);
        }
    }
};

module.exports = CodePathState;
