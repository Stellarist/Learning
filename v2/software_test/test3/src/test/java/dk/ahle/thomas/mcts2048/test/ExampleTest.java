package dk.ahle.thomas.mcts2048.test;

import dk.ahle.thomas.mcts2048.Board;
import dk.ahle.thomas.mcts2048.strategy.Strategy;
import dk.ahle.thomas.mcts2048.strategy.RandomStrategy;
import dk.ahle.thomas.mcts2048.measure.SumMeasure;
import dk.ahle.thomas.mcts2048.measure.FreesMeasure;
import org.junit.jupiter.api.*;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.ValueSource;
import org.junit.jupiter.params.provider.CsvSource;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

import static org.junit.jupiter.api.Assertions.*;
import static org.mockito.Mockito.*;

/**
 * 示例测试类 - 展示JUnit 5和Mockito的使用
 * 
 * 本测试类用于教学目的，演示：
 * 1. JUnit 5基本注解的使用
 * 2. 参数化测试
 * 3. Mockito模拟对象
 * 4. 断言方法
 */
@DisplayName("Board功能测试示例")
public class ExampleTest {

    private Board board;
    
    @Mock
    private Strategy mockStrategy;

    /**
     * 在每个测试方法执行前运行
     */
    @BeforeEach
    public void setUp() {
        MockitoAnnotations.openMocks(this);
        board = new Board();
    }

    /**
     * 在每个测试方法执行后运行
     */
    @AfterEach
    public void tearDown() {
        board = null;
    }

    /**
     * 基本测试示例
     */
    @Test
    @DisplayName("测试新建的Board不为空")
    public void testBoardCreation() {
        assertNotNull(board, "Board对象不应为null");
    }

    /**
     * 测试Board的基本属性
     */
    @Test
    @DisplayName("测试Board的初始状态")
    public void testBoardInitialState() {
        Board newBoard = new Board();
        assertNotNull(newBoard);
        // 可以添加更多关于初始状态的断言
    }

    /**
     * 使用Mockito模拟对象的示例
     */
    // TODO: "Something is wrong with Mockito."
    // @Test
    // @DisplayName("测试Strategy接口的模拟")
    // public void testMockStrategy() {
    //     // 设置mock对象的行为
    //     when(mockStrategy.move(any(Board.class))).thenReturn(Board.UP);
        
    //     // 执行测试
    //     int move = mockStrategy.move(board);
        
    //     // 验证结果
    //     assertEquals(Board.UP, move);
        
    //     // 验证方法被调用
    //     verify(mockStrategy, times(1)).move(any(Board.class));
    // }

    /**
     * 测试异常情况
     */
    @Test
    @DisplayName("测试异常处理")
    public void testExceptionHandling() {
        // 示例：测试某个方法在特定条件下抛出异常
        // assertThrows(IllegalArgumentException.class, () -> {
        //     board.someMethodThatThrowsException();
        // });
        
        // 这里只是示例，实际需要根据代码逻辑调整
        assertDoesNotThrow(() -> {
            Board newBoard = new Board();
        });
    }

    /**
     * 测试Measure功能
     */
    @Test
    @DisplayName("测试SumMeasure")
    public void testSumMeasure() {
        SumMeasure measure = new SumMeasure();
        double score = measure.score(board);
        assertTrue(score >= 0, "分数应该大于等于0");
    }

    /**
     * 测试FreesMeasure功能
     */
    @Test
    @DisplayName("测试FreesMeasure")
    public void testFreesMeasure() {
        FreesMeasure measure = new FreesMeasure();
        double score = measure.score(board);
        assertTrue(score >= 0 && score <= 16, "空闲位置数应该在0-16之间");
    }

    /**
     * 性能测试示例
     */
    // TODO: "Something is wrong with Mockito."
    // @Test
    // @DisplayName("测试RandomStrategy的性能")
    // @Timeout(5) // 超时5秒
    // public void testStrategyPerformance() {
    //     Strategy strategy = new RandomStrategy();
    //     Board testBoard = new Board();
        
    //     // 执行多次移动
    //     for (int i = 0; i < 10; i++) {
    //         int move = strategy.move(testBoard);
    //         testBoard = testBoard.move(move);
    //         if (testBoard.isTerminal()) {
    //             break;
    //         }
    //     }
        
    //     assertTrue(true, "性能测试完成");
    // }

    /**
     * 禁用的测试示例
     */
    @Test
    @Disabled("此测试暂时禁用，等待功能完善")
    @DisplayName("待完善的测试")
    public void testDisabled() {
        fail("这个测试不应该运行");
    }
}
