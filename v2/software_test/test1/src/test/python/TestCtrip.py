import os
import time
import pytest
from datetime import datetime
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.action_chains import ActionChains


@pytest.fixture( scope="function" )
def init_browser():
    service = Service(executable_path="C:/Data/repos/software_test/test1/driver/chromedriver.exe")
    driver = webdriver.Chrome(service=service)
    driver.get("https://www.ctrip.com/")
    driver.maximize_window()
    yield driver
    driver.quit()


def take_screenshot(driver, file_name):
    timestamp = datetime.now().strftime( "%H%M%S%d%f" )
    timestamped_file_name = f"{timestamp}_{file_name}"
    screenshots_dir = "screenshots/python"
    if not os.path.exists( screenshots_dir ):
        os.makedirs( screenshots_dir )
    screenshot_file_path = os.path.join( screenshots_dir, timestamped_file_name )
    driver.save_screenshot( screenshot_file_path )

# ----------------- test_Ctrip_R000 -----------------
def test_Ctrip_R000(init_browser): 
    driver = init_browser

    driver.find_element(By.XPATH, "//*[@id='leftSideNavLayer']/div/div/div[1]/div/div").click()

    actions = ActionChains(driver)
    web_element = driver.find_element(By.XPATH, "//*[@id='leftSideNavLayer']/div/div/div[2]/div/div[1]/div/div[3]/button/span[2]")
    actions.move_to_element(web_element).click().perform()

    depart_input = driver.find_element(By.XPATH, "//*[@id='label-departStation']")
    depart_input.click()
    depart_input.clear()
    depart_input.send_keys("北京")
    time.sleep(1)

    arrive_input = driver.find_element(By.XPATH, "//input[@placeholder='到达城市']")
    arrive_input.click()
    arrive_input.clear()
    arrive_input.send_keys("上海")
    time.sleep(1)

    driver.find_element(By.XPATH, "//*[@id='app']/div[2]/div[1]").click()
    driver.find_element(By.XPATH, "//*[@id='app']/div[2]/div[1]/div[3]/i").click()
    time.sleep(1)

    date_input = driver.find_element(By.XPATH, "//*[@id='app']/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]")
    date_input.click()

    actions = ActionChains(driver)
    actions.move_to_element(date_input).click().perform()

    driver.find_element(By.XPATH, "//*[@id='app']/div[2]/div[1]/div[2]/div[1]/div/div[2]/div[2]/ul[2]/li[5]").click()

    driver.find_element(By.XPATH, "//button[@class='btn-blue btn-search']").click()
    time.sleep(1)

    take_screenshot(driver, "test_Ctrip_R000_001.png")


# ----------------- test_Ctrip_R001 -----------------
def test_Ctrip_R001(init_browser):
    driver = init_browser

    cnt = 0
    departure_cities = ["北京", "上海"]
    arrival_cities = ["广州", "成都"]

    for departure_city in departure_cities:
        for arrival_city in arrival_cities:
            driver.find_element(By.XPATH, "//*[@id='leftSideNavLayer']/div/div/div[1]/div/div").click()
            actions = ActionChains(driver)
            web_element = driver.find_element(By.XPATH,
                "//*[@id='leftSideNavLayer']/div/div/div[2]/div/div[1]/div/div[3]/button")
            actions.move_to_element(web_element).click().perform()

            depart_input = driver.find_element(By.XPATH, "//*[@id='label-departStation']")
            depart_input.click()
            depart_input.clear()
            depart_input.send_keys(departure_city)
            time.sleep(1)

            arrive_input = driver.find_element(By.XPATH, "//input[@placeholder='到达城市']")
            arrive_input.click()
            arrive_input.clear()
            arrive_input.send_keys(arrival_city)
            time.sleep(1)

            if cnt == 0:
                driver.find_element(By.XPATH, "//*[@id='app']/div[2]/div[1]").click()
                driver.find_element(By.XPATH, "//*[@id='app']/div[2]/div[1]/div[3]/i").click()
                time.sleep(1)
                driver.find_element(By.XPATH, "//*[@id='app']/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]").click()

            actions_two = ActionChains(driver)
            element_to_click = driver.find_element(By.XPATH,
                "//*[@id='app']/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]")
            actions_two.move_to_element(element_to_click).click().perform()

            if cnt == 0:
                driver.find_element(By.XPATH,
                    "//*[@id='app']/div[2]/div[1]/div[2]/div[1]/div/div[2]/div[2]/ul[2]/li[5]").click()

            driver.find_element(By.XPATH, "//button[@class='btn-blue btn-search']").click()
            time.sleep(1)

            take_screenshot(driver, f"test_Ctrip_R001_00{cnt + 1}.png")
            cnt += 1


# ----------------- test_Ctrip_R002 -----------------
def test_Ctrip_R002(init_browser):
    driver = init_browser

    driver.find_element(By.XPATH, "//*[@id='leftSideNavLayer']/div/div/div[1]/div/div").click()
    actions = ActionChains(driver)
    web_element = driver.find_element(By.XPATH,
        "//*[@id='leftSideNavLayer']/div/div/div[2]/div/div[1]/div/div[3]/button/span[2]")
    actions.move_to_element(web_element).click().perform()

    depart_input = driver.find_element(By.XPATH, "//*[@id='label-departStation']")
    depart_input.click()
    depart_input.clear()
    depart_input.send_keys("北京")
    time.sleep(1)

    arrive_input = driver.find_element(By.XPATH, "//input[@placeholder='到达城市']")
    arrive_input.click()
    arrive_input.clear()
    arrive_input.send_keys("上海")
    time.sleep(1)

    driver.find_element(By.XPATH, "//*[@id='app']/div[2]/div[1]").click()
    driver.find_element(By.XPATH,
        "//*[@id='app']/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]").click()
    driver.find_element(By.XPATH,
        "//*[@id='app']/div[2]/div[1]/div[2]/div[1]/div/div[2]/div[2]/ul[2]/li[5]").click()

    driver.find_element(By.XPATH, "//button[@class='btn-blue btn-search']").click()
    time.sleep(1)

    driver.find_element(By.XPATH,
        "//*[@id='__next']/div/div[3]/div[2]/div[1]/div[2]/div[1]/ul/li").click()
    driver.find_element(By.XPATH,
        "//*[@id='__next']/div/div[3]/div[1]/div/ul[2]/li[3]/div").click()
    driver.find_element(By.XPATH,
        "//*[@id='__next']/div/div[3]/div[1]/div/ul[2]/li[3]/div").click()

    take_screenshot(driver, "test_Ctrip_R002_001.png")


# ----------------- test_Ctrip_R004 -----------------
def test_Ctrip_R004(init_browser):
    driver = init_browser

    driver.find_element(By.XPATH, "//*[@id='leftSideNavLayer']/div/div/div[1]/div/div").click()
    actions = ActionChains(driver)
    web_element = driver.find_element(By.XPATH,
        "//*[@id='leftSideNavLayer']/div/div/div[2]/div/div[1]/div/div[3]/button/span[2]")
    actions.move_to_element(web_element).click().perform()

    depart_input = driver.find_element(By.XPATH, "//*[@id='label-departStation']")
    depart_input.click()
    depart_input.clear()
    depart_input.send_keys("南京")
    time.sleep(1)

    arrive_input = driver.find_element(By.XPATH, "//input[@placeholder='到达城市']")
    arrive_input.click()
    arrive_input.clear()
    arrive_input.send_keys("广州")
    time.sleep(1)

    driver.find_element(By.XPATH, "//*[@id='app']/div[2]/div[1]").click()
    driver.find_element(By.XPATH,
        "//*[@id='app']/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]").click()
    driver.find_element(By.XPATH,
        "//*[@id='app']/div[2]/div[1]/div[2]/div[1]/div/div[2]/div[2]/ul[2]/li[5]").click()

    driver.find_element(By.XPATH, "//button[@class='btn-blue btn-search']").click()
    time.sleep(1)

    click_paths = [
        "//*[@id='__next']/div/div[3]/div[2]/div[1]/div[3]",
        "//*[@id='__next']/div/div[3]/div[2]/div[1]/div[2]/div[1]/ul/li",
        "//*[@id='__next']/div/div[3]/div[2]/div[1]/div[2]/div[2]/ul/li[1]",
        "//*[@id='__next']/div/div[3]/div[2]/div[1]/div[2]/div[3]/ul/li[1]",
        "//*[@id='__next']/div/div[3]/div[2]/div[1]/div[2]/div[4]/ul/li[1]",
        "//*[@id='__next']/div/div[3]/div[2]/div[1]/div[2]/div[5]/ul/li[1]",
        "//*[@id='__next']/div/div[3]/div[2]/div[1]/div[2]/div[6]/ul/li[1]",
        "//*[@id='__next']/div/div[3]/div[2]/div[1]/div[2]/div[7]/ul/li[1]",
    ]
    for path in click_paths:
        driver.find_element(By.XPATH, path).click()
    take_screenshot(driver, "test_Ctrip_R004_001.png")

    span_paths = [
        "//*[@id='__next']/div/div[3]/div[2]/div[1]/div[2]/div[3]/div/span",
        "//*[@id='__next']/div/div[3]/div[2]/div[1]/div[2]/div[4]/div/span",
        "//*[@id='__next']/div/div[3]/div[2]/div[1]/div[2]/div[5]/div/span",
        "//*[@id='__next']/div/div[3]/div[2]/div[1]/div[2]/div[6]/div/span",
        "//*[@id='__next']/div/div[3]/div[2]/div[1]/div[2]/div[7]/div/span",
        "//*[@id='__next']/div/div[3]/div[2]/div[1]/div[3]"
    ]
    for path in span_paths:
        driver.find_element(By.XPATH, path).click()
    take_screenshot(driver, "test_Ctrip_R004_002.png")

