package dk.ahle.thomas.mcts2048.test;

import java.util.Arrays;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import org.junit.jupiter.api.Test;

import dk.ahle.thomas.mcts2048.Board;
import static dk.ahle.thomas.mcts2048.test.TestUtils.parse;

public class BoardTest2 {
	@Test
	public void testExpected() {
		Board b = parse("1...", "....", "....", "....");
		Board b2 = parse("1111", "1111", "1111", "1111");

		assertFalse(b.isFull());
		assertTrue(b2.isFull());
		assertEquals("Board [grid=1... .... .... .... ]", b.toString());
		assertNotEquals(b.toString(), "Board [grid=1234 5678 9ABC DEFF ]");

		Board b_spawned = b.spawn();

		long tiles_b = Arrays.stream(b.grid()).filter(i -> i > 0).count();
		long tiles_spawned = Arrays.stream(b_spawned.grid()).filter(i -> i > 0).count();
		assertEquals(tiles_b + 1, tiles_spawned);
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

		assertEquals(b1, b2);
		assertNotEquals(b1, b3);
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

	@Test
	public void testUnsafeMove() {
		Board b1 = parse("1111", "....", "....", "....");
		b1.unsafe_move(Board.LEFT);

		assertEquals(parse("22..", "....", "....", "...."), b1);
		assertTrue(b1.changed);

		Board b2 = parse("2111", "....", "....", "....");
		b2.unsafe_move(Board.LEFT);
		assertEquals(parse("221.", "....", "....", "...."), b2);
		assertTrue(b2.changed);

		Board b3 = parse(".123", "....", "....", "....");
		b3.unsafe_move(Board.LEFT);
		assertEquals(parse("123.", "....", "....", "...."), b3);
		assertTrue(b3.changed);

		Board b4 = parse("1234", "....", "....", "....");
		b4.unsafe_move(Board.LEFT);
		assertEquals(parse("1234", "....", "....", "...."), b4);
		assertFalse(b4.changed);
	}

	@Test
	public void testHashCodeAndToString() {
		Board b1 = parse("1234", "....", "....", "....");
		Board b2 = parse("1234", "....", "....", "....");
		Board b3 = parse("2134", "....", "....", "....");

		assertEquals(b1.hashCode(), b2.hashCode());
		assertNotEquals(b1.hashCode(), b3.hashCode());

		assertEquals("Board [grid=1234 .... .... .... ]", b1.toString());
	}
}
