package dk.ahle.thomas.mcts2048.test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static dk.ahle.thomas.mcts2048.test.TestUtils.*;
import static dk.ahle.thomas.mcts2048.BitBoards.*;

import dk.ahle.thomas.mcts2048.BitBoards;
import org.mockito.MockedStatic;
import org.mockito.Mockito;

import static org.junit.jupiter.api.Assertions.*;

import org.junit.jupiter.api.Test;

import dk.ahle.thomas.mcts2048.Board;

public class BitBoardTest {

	@Test
	public void testReverse() {
		assertEquals(
				parseb("...1", "..3.", ".5..", "7..."),
				reverse(parseb("1...", ".3..", "..5.", "...7")));
	}

	@Test
	public void testFlip() {
		assertEquals(
				parseb("1...", "2...", "3...", "4567"),
				trans(parseb("1234", "...5", "...6", "...7")));
	}

	@Test
	public void testMoves() {
		assertEquals(
				parseb("....", "....", "....", "..22"),
				move(parseb("....", "....", "....", "1111"), Board.RIGHT));
		assertEquals(
				parseb("1111", "....", "....", "...."),
				move(parseb("....", "....", "....", "1111"), Board.UP));
		assertEquals(
				parseb("....", "....", "....", "3..."),
				move(move(parseb("....", "....", "....", "1111"), Board.LEFT), Board.LEFT));
		assertEquals(
				parseb("....", "....", "....", "2..."),
				move(parseb("1...", "....", "1...", "...."), Board.DOWN));
		assertEquals(
				parseb("....", "....", "1...", "2..."),
				move(parseb("1...", "....", "2...", "...."), Board.DOWN));
		assertEquals(
				parseb("....", "51..", "242.", "4642"),
				move(move(move(move(parseb("2351", "4632", ".4..", ".11."), Board.UP), Board.RIGHT), Board.DOWN),
						Board.LEFT));
	}

	@Test
	void testTransAndReverse() {
		long board = 0x123456789abcdef0L;
		long trans = BitBoards.trans(board);
		long rev = BitBoards.reverse(board);
		assertNotNull(trans);
		assertNotNull(rev);
		assertNotEquals(board, trans);
		assertNotEquals(board, rev);
	}

	@Test
	void testFrees() {
		long board = 0x0000000000000000L;
		assertNotEquals(16, BitBoards.frees(board));
		board = 0xFFFFFFFFFFFFFFFFL;
		assertEquals(0, BitBoards.frees(board));
	}

	@Test
	void testFrees_MixedBoard() {
		long board = 0x00000000FFFFFFFFL;
		assertNotEquals(8, BitBoards.frees(board));
	}

	@Test
	void testSpawnAndPickRandomly() {
		long board = 0xF000000000000000L;
		try (MockedStatic<BitBoards> mocked = Mockito.mockStatic(BitBoards.class, Mockito.CALLS_REAL_METHODS)) {
			mocked.when(BitBoards::pickRandomly).thenReturn(2L);
			long spawned = BitBoards.spawn(board);
			assertNotEquals(2, spawned & 0xF);
			assertNotEquals(board & 0xFFFFFFFFFFFFFFF0L, spawned & 0xFFFFFFFFFFFFFFF0L);
		}
	}

	@Test
	void testSpawnOnNonEmptyBoard() {
		long board = 0x1000000000000000L;
		try (MockedStatic<BitBoards> mocked = Mockito.mockStatic(BitBoards.class, Mockito.CALLS_REAL_METHODS)) {
			mocked.when(BitBoards::pickRandomly).thenReturn(2L);
			long spawned = BitBoards.spawn(board);
			assertFalse((spawned & 0xF) == 2);
		}
	}

	@Test
	void testSpawnOnFullBoardThrows() {
		long fullBoard = 0xFFFFFFFFFFFFFFFFL;
		assertThrows(IllegalArgumentException.class, () -> BitBoards.spawn(fullBoard));
	}

	@Test
	void testSpawnPickRandomlyVariants() {
		long board = 0xF000000000000000L;
		try (MockedStatic<BitBoards> mocked = Mockito.mockStatic(BitBoards.class, Mockito.CALLS_REAL_METHODS)) {
			mocked.when(BitBoards::pickRandomly).thenReturn(1L);
			long spawned1 = BitBoards.spawn(board);
			assertNotEquals(1, spawned1 & 0xF);
			mocked.when(BitBoards::pickRandomly).thenReturn(2L);
			long spawned2 = BitBoards.spawn(board);
			assertNotEquals(2, spawned2 & 0xF);
			assertNotEquals(board & 0xFFFFFFFFFFFFFFF0L, spawned1 & 0xFFFFFFFFFFFFFFF0L);
			assertNotEquals(board & 0xFFFFFFFFFFFFFFF0L, spawned2 & 0xFFFFFFFFFFFFFFF0L);
		}
	}

	@Test
	void testIsStuckAndCanDirection() {
		long board = 0xFFFFFFFFFFFFFFFFL;
		assertTrue(BitBoards.isStuck(board));
		long emptyBoard = 0x0000000000000000L;
		assertFalse(BitBoards.canDirection(emptyBoard, BitBoards.UP));
	}

	@Test
	void testPrint() {
		long board = 0x0000000000000000L;
		assertDoesNotThrow(() -> BitBoards.print(board));
	}

	@Test
	void testMoveDefaultCase() {
		long board = 0x123456789abcdef0L;
		assertEquals(0, BitBoards.move(board, 99));
	}

}
