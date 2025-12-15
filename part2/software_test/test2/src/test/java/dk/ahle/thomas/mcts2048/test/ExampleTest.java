package dk.ahle.thomas.mcts2048.test;

import dk.ahle.thomas.mcts2048.Board;
import dk.ahle.thomas.mcts2048.strategy.Strategy;
import dk.ahle.thomas.mcts2048.measure.SumMeasure;
import dk.ahle.thomas.mcts2048.measure.FreesMeasure;
import org.junit.jupiter.api.*;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

import static org.junit.jupiter.api.Assertions.*;
import static org.mockito.Mockito.*;

@DisplayName("Board功能测试示例")
public class ExampleTest {
    private Board board;

    @Mock
    private Strategy mockStrategy;

    @BeforeEach
    public void setUp() {
        MockitoAnnotations.openMocks(this);
        board = new Board();
    }

    @AfterEach
    public void tearDown() {
        board = null;
    }

    @Test
    @DisplayName("测试新建的Board不为空")
    public void testBoardCreation() {
        assertNotNull(board, "Board对象不应为null");
    }

    @Test
    @DisplayName("测试Board的初始状态")
    public void testBoardInitialState() {
        Board newBoard = new Board();
        assertNotNull(newBoard);
        // 可以添加更多关于初始状态的断言
    }

    @Test
    @DisplayName("测试Strategy接口的模拟")
    public void testMockStrategy() {
        when(mockStrategy.play(any(Board.class))).thenReturn(board);

        Board result = mockStrategy.play(board);

        assertEquals(board, result);

        verify(mockStrategy, times(1)).play(any(Board.class));
    }

    @Test
    @DisplayName("测试异常处理")
    public void testExceptionHandling() {
        // assertThrows(IllegalArgumentException.class, () -> {
        // board.someMethodThatThrowsException();
        // });

        // 这里只是示例，实际需要根据代码逻辑调整
        assertDoesNotThrow(() -> {
            new Board();
        });
    }

    @Test
    @DisplayName("测试SumMeasure")
    public void testSumMeasure() {
        SumMeasure measure = new SumMeasure();
        double score = measure.score(board);
        assertTrue(score >= 0, "分数应该大于等于0");
    }

    @Test
    @DisplayName("测试FreesMeasure")
    public void testFreesMeasure() {
        FreesMeasure measure = new FreesMeasure();
        double score = measure.score(board);
        assertTrue(score >= 0 && score <= 16, "空闲位置数应该在0-16之间");
    }

    // @Test
    // @DisplayName("测试RandomStrategy的性能")
    // @Timeout(5) // 超时5秒
    // public void testStrategyPerformance() {
    // Strategy strategy = new RandomStrategy();
    // Board testBoard = new Board();

    // // 执行多次移动
    // for (int i = 0; i < 10; i++) {
    // int move = strategy.move(testBoard);
    // testBoard = testBoard.move(move);
    // if (testBoard.isTerminal()) {
    // break;
    // }
    // }

    // assertTrue(true, "性能测试完成");
    // }

    @Test
    @Disabled("此测试暂时禁用，等待功能完善")
    @DisplayName("待完善的测试")
    public void testDisabled() {
        fail("这个测试不应该运行");
    }
}
