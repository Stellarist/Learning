package dk.ahle.thomas.mcts2048.test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import org.junit.jupiter.api.Test;

import dk.ahle.thomas.mcts2048.Board;
import static dk.ahle.thomas.mcts2048.test.TestUtils.parse;

public class BoardTest {
	@Test
	public void testExpected() {
		Board b = parse("1...", "....", "....", "....");
		Board b2 = parse("1111", "1111", "1111", "1111");
		assertFalse(b.isFull());
		assertTrue(b2.isFull());
		assertNotEquals(b.toString(), "Board [grid=1234 5678 9ABC DEFF ]");
		b.spawn().unsafe_spawn();
		b.print();

		assertTrue(b.canDirection(Board.UP));
		assertTrue(b.canDirection(Board.DOWN));
		assertTrue(b.canDirection(Board.LEFT));
		assertTrue(b.canDirection(Board.RIGHT));
	}

	@Test
	public void testEquals() {
		Board b1 = parse("1...", "....", "....", "....");
		Board b2 = parse("1...", "....", "....", "....");
		Board b3 = parse("2...", "....", "....", "....");
		assertEquals(
				parse("1...", "....", "....", "...."),
				parse("1...", "....", "....", "...."));
		assertNotEquals(
				parse("1...", "....", "....", "...."),
				parse("2...", "....", "....", "...."));
		assertTrue(b1.equals(b1));
		assertTrue(b1.equals(b2));
		assertFalse(b1.equals(null));
		assertFalse(b1.equals("A string"));
		assertFalse(b1.equals(b3));
	}

	@Test
	public void testMoves() {
		Board b = parse("1...", "....", "....", "....");
		Board b1 = parse("1...", "....", "....", "....");
		Board b2 = parse("1123", "4567", "89AB", "CDEF");
		Board b3 = parse("1212", "2121", "1212", "2121");

		assertFalse(b1.isStuck());
		assertFalse(b2.isStuck());
		assertTrue(b3.isStuck());

		assertEquals(
				parse("1111", "....", "....", "...."),
				parse("....", "....", "....", "1111").move(Board.UP));
		assertEquals(
				parse("....", "....", "....", "..22"),
				parse("....", "....", "....", "1111").move(Board.RIGHT));
		assertEquals(
				parse("....", "....", "....", "3..."),
				parse("....", "....", "....", "1111").move(Board.LEFT).move(Board.LEFT));
		assertEquals(
				parse("....", "....", "....", "2..."),
				parse("1...", "....", "1...", "....").move(Board.DOWN));
		assertEquals(
				parse("....", "....", "1...", "2..."),
				parse("1...", "....", "2...", "....").move(Board.DOWN));
		b.unsafe_move(1);
	}
}
