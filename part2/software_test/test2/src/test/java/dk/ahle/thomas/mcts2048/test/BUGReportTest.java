package dk.ahle.thomas.mcts2048.test;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

import dk.ahle.thomas.mcts2048.BitBoards;

/**
 * Regression tests highlighting defects discovered in the production code.
 */
public class BUGReportTest {

    @Test
    void bitBoardsIsStuckReturnsTrueWhenMovesExist() {
        long board = TestUtils.parseb(
                "1...",
                "....",
                "....",
                "....");

        assertTrue(BitBoards.canDirection(board, BitBoards.LEFT),
                "Board has a legal move to the left and should not be considered stuck.");

        assertFalse(BitBoards.isStuck(board),
                "BitBoards.isStuck should return false when at least one move is available, but it currently returns true.");
    }
}
