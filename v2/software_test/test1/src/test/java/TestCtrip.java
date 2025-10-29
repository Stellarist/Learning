import org.apache.commons.io.FileUtils;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.openqa.selenium.*;
import org.openqa.selenium.chrome.ChromeDriver;
import org.openqa.selenium.chrome.ChromeOptions;
import org.openqa.selenium.interactions.Actions;
import org.openqa.selenium.support.PageFactory;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;

public class TestCtrip {
    private WebDriver driver;

    @BeforeEach
    public void setup() {
        // Windows：以下路径改为你本地到webdriver路径
        String driver_path = "C:/Data/repos/software_test/test1/driver/chromedriver.exe";
        System.setProperty("webdriver.chrome.driver", driver_path);
        ChromeOptions chromeOptions = new ChromeOptions();
        chromeOptions.addArguments("--remote-allow-origins=*");
        driver = new ChromeDriver(chromeOptions);
        driver.get("https://www.ctrip.com");
        driver.manage().window().maximize();
    }

    @Test
    public void test_Ctrip_R000() throws InterruptedException {
        // 初始化drvier
        PageFactory.initElements(driver, this);
        // 点击左上角展开按钮
        driver.findElement(By.xpath("//*[@id=\"leftSideNavLayer\"]/div/div/div[1]/div/div")).click();
        // 鼠标悬浮到火车票按钮，点击
        Actions actions = new Actions(driver);
        WebElement webElement = driver.findElement(By.xpath("//*[@id=\"leftSideNavLayer\"]/div/div/div[2]/div/div[1]/div/div[3]/button/span[2]"));
        actions.moveToElement(webElement).click().perform();

        // step3 输入出发城市“北京”
        driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).click();
        driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).clear();
        driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).sendKeys("北京");
        Thread.sleep(1000);

        // step4 输入到达城市“上海”
        driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).click();
        driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).clear();
        driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).sendKeys("上海");
        Thread.sleep(1000);
        // step5 勾选【只搜高铁动车】选择框

        driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]")).click();
        driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[3]/i")).click();
        // 这里需要加一个强制等待,不然系统反应不过来报错
        Thread.sleep(1000);

        // 注意:这里是这个小坑 需再次点击一下日期控件以激活它
        driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]")).click();

        // step6 模拟鼠标事件点击日期控件
        Actions actionsTwo = new Actions(driver);
        actionsTwo.moveToElement(driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]"))).click().perform();

        // step7 选择出发日期为2025-10-02
        driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[2]/div[1]/div/div[2]/div[2]/ul[2]/li[5]")).click();

        // step8 点击搜索按钮
        driver.findElement(By.xpath("//button[@class=\"btn-blue btn-search\"]")).click();
        Thread.sleep(1000);

        // step9 截图
        takeScreenshot("test_Ctrip_R000_001.png");
    }

    @Test
    public void test_Ctrip_R001() throws InterruptedException {
        int cnt = 0;
        String[] departure_cities = {"北京", "上海"};
        String[] arrival_cities = {"广州", "成都"};

        PageFactory.initElements(driver, this);

        for (String departure_city : departure_cities) {
            for (String arrival_city : arrival_cities) {
                driver.findElement(By.xpath("//*[@id=\"leftSideNavLayer\"]/div/div/div[1]/div/div")).click();
                Actions actions = new Actions(driver);
                WebElement webElement = driver.findElement(By.xpath("//*[@id=\"leftSideNavLayer\"]/div/div/div[2]/div/div[1]/div/div[3]/button"));
                actions.moveToElement(webElement).click().perform();

                driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).click();
                driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).clear();
                driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).sendKeys(departure_city);
                Thread.sleep(1000);

                driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).click();
                driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).clear();
                driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).sendKeys(arrival_city);
                Thread.sleep(1000);

                if(cnt == 0) {
                    driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]")).click();
                    driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[3]/i")).click();
                    Thread.sleep(1000);

                    driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]")).click();
                }

                Actions actionsTwo = new Actions(driver);
                actionsTwo.moveToElement(driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]"))).click().perform();

                if(cnt == 0) {
                    driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[2]/div[1]/div/div[2]/div[2]/ul[2]/li[5]")).click();
                }

                driver.findElement(By.xpath("//button[@class=\"btn-blue btn-search\"]")).click();
                Thread.sleep(1000);

                takeScreenshot("test_Ctrip_R001_00" + Integer.toString(cnt + 1) + ".png");
                cnt++;
            }
        }
    }

    @Test
    public void test_Ctrip_R002() throws InterruptedException {
        PageFactory.initElements(driver, this);
        driver.findElement(By.xpath("//*[@id=\"leftSideNavLayer\"]/div/div/div[1]/div/div")).click();
        Actions actions = new Actions(driver);
        WebElement webElement = driver.findElement(By.xpath("//*[@id=\"leftSideNavLayer\"]/div/div/div[2]/div/div[1]/div/div[3]/button/span[2]"));
        actions.moveToElement(webElement).click().perform();

        driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).click();
        driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).clear();
        driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).sendKeys("北京");
        Thread.sleep(1000);

        driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).click();
        driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).clear();
        driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).sendKeys("上海");
        Thread.sleep(1000);

        driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]")).click();
        driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]")).click();
        driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[2]/div[1]/div/div[2]/div[2]/ul[2]/li[5]")).click();

        driver.findElement(By.xpath("//button[@class=\"btn-blue btn-search\"]")).click();
        Thread.sleep(1000);

        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div[2]/div[1]/div[2]/div[1]/ul/li")).click();

        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div[1]/div/ul[2]/li[3]/div")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div[1]/div/ul[2]/li[3]/div")).click();

        takeScreenshot("test_Ctrip_R002_001.png");
    }

    @Test
    public void test_Ctrip_R004() throws InterruptedException {
        PageFactory.initElements(driver, this);
        driver.findElement(By.xpath("//*[@id=\"leftSideNavLayer\"]/div/div/div[1]/div/div")).click();
        Actions actions = new Actions(driver);
        WebElement webElement = driver.findElement(By.xpath("//*[@id=\"leftSideNavLayer\"]/div/div/div[2]/div/div[1]/div/div[3]/button/span[2]"));
        actions.moveToElement(webElement).click().perform();

        driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).click();
        driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).clear();
        driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).sendKeys("南京");
        Thread.sleep(1000);

        driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).click();
        driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).clear();
        driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).sendKeys("广州");
        Thread.sleep(1000);

        driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]")).click();
        driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]")).click();
        driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[2]/div[1]/div/div[2]/div[2]/ul[2]/li[5]")).click();

        driver.findElement(By.xpath("//button[@class=\"btn-blue btn-search\"]")).click();
        Thread.sleep(1000);

        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div[2]/div[1]/div[3]")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div[2]/div[1]/div[2]/div[1]/ul/li")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div[2]/div[1]/div[2]/div[2]/ul/li[1]")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div[2]/div[1]/div[2]/div[3]/ul/li[1]")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div[2]/div[1]/div[2]/div[4]/ul/li[1]")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div[2]/div[1]/div[2]/div[5]/ul/li[1]")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div[2]/div[1]/div[2]/div[6]/ul/li[1]")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div[2]/div[1]/div[2]/div[7]/ul/li[1]")).click();
        takeScreenshot("test_Ctrip_R004_001.png");

        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div[2]/div[1]/div[2]/div[3]/div/span")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div[2]/div[1]/div[2]/div[4]/div/span")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div[2]/div[1]/div[2]/div[5]/div/span")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div[2]/div[1]/div[2]/div[6]/div/span")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div[2]/div[1]/div[2]/div[7]/div/span")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div[2]/div[1]/div[3]")).click();
        takeScreenshot("test_Ctrip_R004_002.png");
    }

    @AfterEach
    public void teardown() {
        this.driver.quit();
    }

    private void takeScreenshot(String fileName) {
        SimpleDateFormat dateFormat = new SimpleDateFormat("HHmmssddSSS");
        String timestamp = dateFormat.format(new Date());
        String timestampedFileName = timestamp + "_" + fileName;
        File screenshotsDir = new File("screenshots/java");
        if (!screenshotsDir.exists()) {
            screenshotsDir.mkdirs();
        }
        String screenshotFilePath = screenshotsDir.getAbsolutePath() + File.separator + timestampedFileName;
        File screenshotFile = ((TakesScreenshot) driver).getScreenshotAs(OutputType.FILE);
        try {
            FileUtils.copyFile(screenshotFile, new File(screenshotFilePath));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
