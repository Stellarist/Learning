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
    public void test_Ctrip_R005() throws InterruptedException {
        int cnt = 0;
        String[] departure_cities = {"济南", "天津"};
        String[] arrival_cities = {"西安", "杭州"};

        PageFactory.initElements(driver, this);

        for (String departure_city : departure_cities) {
            for (String arrival_city : arrival_cities) {
                driver.findElement(By.xpath("//*[@id=\"leftSideNavLayer\"]/div/div/div[1]/div/div")).click();
                Actions actions = new Actions(driver);
                WebElement webElement = driver.findElement(By.xpath("//*[@id=\"leftSideNavLayer\"]/div/div/div[2]/div/div[1]/div/div[3]/button"));
                actions.moveToElement(webElement).click().perform();

                driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[1]/div/ul/li[2]")).click();

                driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).click();
                driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).clear();
                driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).sendKeys(departure_city);
                Thread.sleep(1000);

                driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).click();
                driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).clear();
                driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).sendKeys(arrival_city);
                Thread.sleep(1000);

                driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]")).click();
                driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]")).click();

                Actions actionsTwo = new Actions(driver);
                actionsTwo.moveToElement(driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]"))).click().perform();

                if(cnt == 0)
                    driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[2]/div[1]/div/div[2]/div[2]/ul[2]/li[5]")).click();

                driver.findElement(By.xpath("//button[@class=\"btn-blue btn-search\"]")).click();
                Thread.sleep(1000);

                driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[1]/div[2]/div[3]/div")).click();
                Thread.sleep(1000);

                driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[1]/div[1]")).click();
                driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[2]/div[2]")).click();
                driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[3]/div[2]")).click();
                driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[5]/div[1]")).click();
                if(cnt != 3)
                    driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[6]/div[1]")).click();

                driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[1]/div[2]/div[3]/div")).click();
                Thread.sleep(1000);

                takeScreenshot("test_Ctrip_R005_00" + Integer.toString(cnt + 1) + ".png");
                cnt++;
            }
        }
    }

        @Test
    public void test_Ctrip_R006() throws InterruptedException {
        int cnt = 0;
        String[] departure_cities = {"兰州", "徐州"};
        String[] arrival_cities = {"郑州", "厦门"};

        PageFactory.initElements(driver, this);

        for (String departure_city : departure_cities) {
            for (String arrival_city : arrival_cities) {
                driver.findElement(By.xpath("//*[@id=\"leftSideNavLayer\"]/div/div/div[1]/div/div")).click();
                Actions actions = new Actions(driver);
                WebElement webElement = driver.findElement(By.xpath("//*[@id=\"leftSideNavLayer\"]/div/div/div[2]/div/div[1]/div/div[3]/button"));
                actions.moveToElement(webElement).click().perform();

                driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[1]/div/ul/li[2]")).click();

                driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).click();
                driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).clear();
                driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).sendKeys(departure_city);
                Thread.sleep(1000);

                driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).click();
                driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).clear();
                driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).sendKeys(arrival_city);
                Thread.sleep(1000);

                driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]")).click();
                driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]")).click();

                Actions actionsTwo = new Actions(driver);
                actionsTwo.moveToElement(driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]"))).click().perform();

                if(cnt == 0)
                    driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[2]/div[1]/div/div[2]/div[2]/ul[2]/li[5]")).click();

                driver.findElement(By.xpath("//button[@class=\"btn-blue btn-search\"]")).click();
                Thread.sleep(1000);

                driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[3]/div[2]/div[3]/div")).click();
                Thread.sleep(1000);

                driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[3]/div[2]/div[3]/ul/li[1]/div[1]")).click();
                driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[3]/div[2]/div[3]/ul/li[2]/div[2]")).click();
                driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[3]/div[2]/div[3]/ul/li[3]/div[2]")).click();
                driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[3]/div[2]/div[3]/ul/li[5]/div[1]")).click();
                driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[3]/div[2]/div[3]/ul/li[6]/div[1]")).click();

                driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[3]/div[2]/div[3]/div")).click();
                Thread.sleep(1000);

                takeScreenshot("test_Ctrip_R006_00" + Integer.toString(cnt + 1) + ".png");
                cnt++;
            }
        }
    }

    @Test
    public void test_Ctrip_R007() throws InterruptedException {
        PageFactory.initElements(driver, this);
        driver.findElement(By.xpath("//*[@id=\"leftSideNavLayer\"]/div/div/div[1]/div/div")).click();
        Actions actions = new Actions(driver);
        WebElement webElement = driver.findElement(By.xpath("//*[@id=\"leftSideNavLayer\"]/div/div/div[2]/div/div[1]/div/div[3]/button/span[2]"));
        actions.moveToElement(webElement).click().perform();

        driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[1]/div/ul/li[2]")).click();

        driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).click();
        driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).clear();
        driver.findElement(By.xpath("//*[@id=\"label-departStation\"]")).sendKeys("123456");
        Thread.sleep(1000);

        driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).click();
        driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).clear();
        driver.findElement(By.xpath("//input[@placeholder=\"到达城市\"]")).sendKeys("海口");
        Thread.sleep(1000);

        driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]")).click();
        driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]")).click();
        driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[2]/div[1]/div/div[2]/div[2]/ul[2]/li[5]")).click();

        driver.findElement(By.xpath("//button[@class=\"btn-blue btn-search\"]")).click();
        Thread.sleep(1000);

        takeScreenshot("test_Ctrip_R007_001.png");
    }

    @Test
    public void test_Ctrip_R008() throws InterruptedException {
        PageFactory.initElements(driver, this);
        driver.findElement(By.xpath("//*[@id=\"leftSideNavLayer\"]/div/div/div[1]/div/div")).click();
        Actions actions = new Actions(driver);
        WebElement webElement = driver.findElement(By.xpath("//*[@id=\"leftSideNavLayer\"]/div/div/div[2]/div/div[1]/div/div[3]/button/span[2]"));
        actions.moveToElement(webElement).click().perform();

        driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[1]/div/ul/li[2]")).click();
        driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[3]")).click();
        Thread.sleep(1000);

        driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]")).click();
        driver.findElement(By.xpath("//*[@id=\"app\"]/div[2]/div[1]/div[2]/div[1]/div/div[2]/div[1]/ul[2]/li[31]")).click();

        driver.findElement(By.xpath("//button[@class=\"btn-blue btn-search\"]")).click();
        Thread.sleep(1000);

        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[1]/div[2]/ul[1]/li[3]")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[1]/div[2]/div[3]/div")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[2]/div[1]")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[2]/div[2]")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[2]/div[3]")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[2]/div[4]")).click();
        Thread.sleep(1000);

        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[3]/div[2]/div[3]/div")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[3]/div[1]")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[4]/div[1]")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[5]/div[1]")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[6]/div[1]")).click();
        Thread.sleep(1000);

        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[3]/div[2]/div[3]/ul/li[4]/div[1]")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[3]/div[2]/div[3]/ul/li[5]/div[1]")).click();
        driver.findElement(By.xpath("//*[@id=\"__next\"]/div/div[3]/div/div[3]/div[2]/div[3]/ul/li[6]/div[1]")).click();
        Thread.sleep(1000);

        takeScreenshot("test_Ctrip_R008_001.png");
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
