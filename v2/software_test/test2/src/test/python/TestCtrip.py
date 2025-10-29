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


# ----------------- test_Ctrip_R005 -----------------
def test_Ctrip_R005(init_browser):
    driver = init_browser
    cnt = 0
    departure_cities = ["济南", "天津"]
    arrival_cities = ["西安", "杭州"]

    for departure_city in departure_cities:
        for arrival_city in arrival_cities:
            driver.find_element(By.XPATH, "//*[@id='leftSideNavLayer']/div/div/div[1]/div/div").click()
            actions = ActionChains(driver)
            web_element = driver.find_element(By.XPATH,
                "//*[@id='leftSideNavLayer']/div/div/div[2]/div/div[1]/div/div[3]/button")
            actions.move_to_element(web_element).click().perform()

            driver.find_element(By.XPATH, "//*[@id='app']/div[2]/div[1]/div[1]/div/ul/li[2]").click()

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

            driver.find_element(By.XPATH, "//*[@id='app']/div[2]/div[1]").click()
            date_input = driver.find_element(By.XPATH,
                "//*[@id='app']/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]")
            date_input.click()

            actions_two = ActionChains(driver)
            actions_two.move_to_element(date_input).click().perform()

            if cnt == 0:
                driver.find_element(By.XPATH,
                    "//*[@id='app']/div[2]/div[1]/div[2]/div[1]/div/div[2]/div[2]/ul[2]/li[5]").click()

            driver.find_element(By.XPATH, "//button[@class='btn-blue btn-search']").click()
            time.sleep(1)

            driver.find_element(By.XPATH,
                "//*[@id='__next']/div/div[3]/div/div[1]/div[2]/div[3]/div").click()
            time.sleep(1)

            click_paths = [
                "//*[@id='__next']/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[1]/div[1]",
                "//*[@id='__next']/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[2]/div[2]",
                "//*[@id='__next']/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[3]/div[2]",
                "//*[@id='__next']/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[5]/div[1]",
            ]
            for path in click_paths:
                driver.find_element(By.XPATH, path).click()
            if cnt != 3:
                driver.find_element(By.XPATH,
                    "//*[@id='__next']/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[6]/div[1]").click()

            driver.find_element(By.XPATH,
                "//*[@id='__next']/div/div[3]/div/div[1]/div[2]/div[3]/div").click()
            time.sleep(1)

            take_screenshot(driver, f"test_Ctrip_R005_00{cnt + 1}.png")
            cnt += 1


# ----------------- test_Ctrip_R006 -----------------
def test_Ctrip_R006(init_browser):
    driver = init_browser
    cnt = 0
    departure_cities = ["兰州", "徐州"]
    arrival_cities = ["郑州", "厦门"]

    for departure_city in departure_cities:
        for arrival_city in arrival_cities:
            driver.find_element(By.XPATH, "//*[@id='leftSideNavLayer']/div/div/div[1]/div/div").click()
            actions = ActionChains(driver)
            web_element = driver.find_element(By.XPATH,
                "//*[@id='leftSideNavLayer']/div/div/div[2]/div/div[1]/div/div[3]/button")
            actions.move_to_element(web_element).click().perform()

            driver.find_element(By.XPATH, "//*[@id='app']/div[2]/div[1]/div[1]/div/ul/li[2]").click()

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

            driver.find_element(By.XPATH, "//*[@id='app']/div[2]/div[1]").click()
            date_input = driver.find_element(By.XPATH,
                "//*[@id='app']/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]")
            date_input.click()

            actions_two = ActionChains(driver)
            actions_two.move_to_element(date_input).click().perform()

            if cnt == 0:
                driver.find_element(By.XPATH,
                    "//*[@id='app']/div[2]/div[1]/div[2]/div[1]/div/div[2]/div[2]/ul[2]/li[5]").click()

            driver.find_element(By.XPATH, "//button[@class='btn-blue btn-search']").click()
            time.sleep(1)

            driver.find_element(By.XPATH,
                "//*[@id='__next']/div/div[3]/div/div[3]/div[2]/div[3]/div").click()
            time.sleep(1)

            click_paths = [
                "//*[@id='__next']/div/div[3]/div/div[3]/div[2]/div[3]/ul/li[1]/div[1]",
                "//*[@id='__next']/div/div[3]/div/div[3]/div[2]/div[3]/ul/li[2]/div[2]",
                "//*[@id='__next']/div/div[3]/div/div[3]/div[2]/div[3]/ul/li[3]/div[2]",
                "//*[@id='__next']/div/div[3]/div/div[3]/div[2]/div[3]/ul/li[5]/div[1]",
                "//*[@id='__next']/div/div[3]/div/div[3]/div[2]/div[3]/ul/li[6]/div[1]",
            ]
            for path in click_paths:
                driver.find_element(By.XPATH, path).click()

            driver.find_element(By.XPATH,
                "//*[@id='__next']/div/div[3]/div/div[3]/div[2]/div[3]/div").click()
            time.sleep(1)

            take_screenshot(driver, f"test_Ctrip_R006_00{cnt + 1}.png")
            cnt += 1


# ----------------- test_Ctrip_R007 -----------------
def test_Ctrip_R007(init_browser):
    driver = init_browser

    driver.find_element(By.XPATH, "//*[@id='leftSideNavLayer']/div/div/div[1]/div/div").click()
    actions = ActionChains(driver)
    web_element = driver.find_element(By.XPATH,
        "//*[@id='leftSideNavLayer']/div/div/div[2]/div/div[1]/div/div[3]/button/span[2]")
    actions.move_to_element(web_element).click().perform()

    driver.find_element(By.XPATH, "//*[@id='app']/div[2]/div[1]/div[1]/div/ul/li[2]").click()

    depart_input = driver.find_element(By.XPATH, "//*[@id='label-departStation']")
    depart_input.click()
    depart_input.clear()
    depart_input.send_keys("123456")
    time.sleep(1)

    arrive_input = driver.find_element(By.XPATH, "//input[@placeholder='到达城市']")
    arrive_input.click()
    arrive_input.clear()
    arrive_input.send_keys("海口")
    time.sleep(1)

    driver.find_element(By.XPATH, "//*[@id='app']/div[2]/div[1]").click()
    driver.find_element(By.XPATH,
        "//*[@id='app']/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]").click()
    driver.find_element(By.XPATH,
        "//*[@id='app']/div[2]/div[1]/div[2]/div[1]/div/div[2]/div[2]/ul[2]/li[5]").click()

    driver.find_element(By.XPATH, "//button[@class='btn-blue btn-search']").click()
    time.sleep(1)

    take_screenshot(driver, "test_Ctrip_R007_001.png")


# ----------------- test_Ctrip_R008 -----------------
def test_Ctrip_R008(init_browser):
    driver = init_browser

    driver.find_element(By.XPATH, "//*[@id='leftSideNavLayer']/div/div/div[1]/div/div").click()
    actions = ActionChains(driver)
    web_element = driver.find_element(By.XPATH,
        "//*[@id='leftSideNavLayer']/div/div/div[2]/div/div[1]/div/div[3]/button/span[2]")
    actions.move_to_element(web_element).click().perform()

    driver.find_element(By.XPATH, "//*[@id='app']/div[2]/div[1]/div[1]/div/ul/li[2]").click()
    driver.find_element(By.XPATH, "//*[@id='app']/div[2]/div[1]/div[3]").click()
    time.sleep(1)

    driver.find_element(By.XPATH,
        "//*[@id='app']/div[2]/div[1]/div[2]/div[4]/div[1]/div[1]").click()
    driver.find_element(By.XPATH,
        "//*[@id='app']/div[2]/div[1]/div[2]/div[1]/div/div[2]/div[1]/ul[2]/li[31]").click()

    driver.find_element(By.XPATH, "//button[@class='btn-blue btn-search']").click()
    time.sleep(1)

    driver.find_element(By.XPATH, "//*[@id='__next']/div/div[3]/div/div[1]/div[2]/ul[1]/li[3]").click()
    driver.find_element(By.XPATH, "//*[@id='__next']/div/div[3]/div/div[1]/div[2]/div[3]/div").click()
    click_paths = [
        "//*[@id='__next']/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[2]/div[1]",
        "//*[@id='__next']/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[2]/div[2]",
        "//*[@id='__next']/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[2]/div[3]",
        "//*[@id='__next']/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[2]/div[4]",
    ]
    for path in click_paths:
        driver.find_element(By.XPATH, path).click()
    time.sleep(1)

    driver.find_element(By.XPATH, "//*[@id='__next']/div/div[3]/div/div[3]/div[2]/div[3]/div").click()
    click_paths2 = [
        "//*[@id='__next']/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[3]/div[1]",
        "//*[@id='__next']/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[4]/div[1]",
        "//*[@id='__next']/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[5]/div[1]",
        "//*[@id='__next']/div/div[3]/div/div[1]/div[2]/div[3]/ul/li[6]/div[1]",
    ]
    for path in click_paths2:
        driver.find_element(By.XPATH, path).click()
    time.sleep(1)

    click_paths3 = [
        "//*[@id='__next']/div/div[3]/div/div[3]/div[2]/div[3]/ul/li[4]/div[1]",
        "//*[@id='__next']/div/div[3]/div/div[3]/div[2]/div[3]/ul/li[5]/div[1]",
        "//*[@id='__next']/div/div[3]/div/div[3]/div[2]/div[3]/ul/li[6]/div[1]",
    ]
    for path in click_paths3:
        driver.find_element(By.XPATH, path).click()
    time.sleep(1)

    take_screenshot(driver, "test_Ctrip_R008_001.png")
