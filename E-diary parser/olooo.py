import vk_api
from vk_api.longpoll import VkLongPoll, VkEventType
import random
import json
from selenium import webdriver
import threading
import copy
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import NoSuchElementException, TimeoutException
import selenium.webdriver.chrome.options
import selenium.webdriver.firefox.options
from datetime import datetime
from pytz import timezone
from bs4 import BeautifulSoup
import math
import time

DEBUG = False

def sendMessage(user, text, kbd = False, week = 1):
    if len(text) >= 4000:
        sendMessage(<CONFIDENTIAL INFO>, "TOO LONG MESSAGE " + str(user), True)
        return

    if kbd:

        keyboard = {

            "one_time" : False,

            "buttons" :
            [

                [
                    {
                        "action" : {
                            "type" : "text",
                            "label" : "Текущий урок"
                        },
                        "color" : "primary"
                    },

                    {
                        "action" : {
                            "type" : "text",
                            "label" : "След. урок"
                        },
                        "color" : "primary"
                    }

                ],

                [   
                    {
                        "action" : {
                            "type" : "text",
                            "label" : "Сегодня"
                        },
                        "color" : "primary"
                    },

                    {
                        "action" : {
                            "type" : "text",
                            "label" : "Завтра"
                        },
                        "color" : "primary"
                    }
                ],


                [
                    {
                        "action" : {
                            "type" : "text",
                            "label" : "Звонки"
                        },
                        "color" : "secondary"
                    },

                    {
                        "action" : {
                            "type" : "text",
                            "label" : "Оценки"
                        },
                        "color" : "positive"
                    },
                

                    {
                        "action" : {
                            "type" : "text",
                            "label" : "Помощь"
                        },
                        "color" : "secondary"
                    }
                ],


                [
                    {
                        "action" : {
                            "type" : "text",
                            "label" : "Прошлая неделя"
                        },
                        "color" : "negative"
                    },

                    {
                        "action" : {
                            "type" : "text",
                            "label" : "Эта неделя"
                        },
                        "color" : "negative"
                    },

                    {
                        "action" : {
                            "type" : "text",
                            "label" : "След. неделя"
                        },
                        "color" : "negative"
                    }

                ],
                

                [

                    {
                        "action" : {
                            "type" : "text",
                            "label" : "вся нед."
                        },
                        "color" : "primary"
                    },

                    {
                        "action" : {
                            "type" : "text",
                            "label" : "пн"
                        },
                        "color" : "secondary"
                    },

                    {
                        "action" : {
                            "type" : "text",
                            "label" : "вт"
                        },
                        "color" : "secondary"
                    }

                ],

                [

                    {
                        "action" : {
                            "type" : "text",
                            "label" : "ср"
                        },
                        "color" : "secondary"
                    },

                    {
                        "action" : {
                            "type" : "text",
                            "label" : "чт"
                        },
                        "color" : "secondary"
                    },

                    {
                        "action" : {
                            "type" : "text",
                            "label" : "пт"
                        },
                        "color" : "secondary"
                    }
                ]
                
            ]
        }

        keyboard['buttons'][3][week]['color'] = 'positive'

    else:

        keyboard = {
            "one_time" : True,

            "buttons" : []
        }


    keyboard = json.dumps(keyboard)

    vkOthMutex.acquire()

    try:
        vk_other.messages.send(user_id = user, message = text,
            random_id = random.randint(1, 10 ** 18), keyboard = keyboard)
    except vk_api.exceptions.ApiError:
        pass

    vkOthMutex.release()

def databaseGet(data, mutex, key):
    mutex.acquire()

    try:
        ans = data[key]
    except KeyError:
        ans = None

    mutex.release()
    return ans

def databaseRemove(data, mutex, key):
    mutex.acquire()

    data[key] = None

    mutex.release()


def databasePut(data, mutex, key, value):
    mutex.acquire()

    data[key] = value

    mutex.release()


def schoolsByAuthorize(browser):
    
    browser.get(<CONFIDENTIAL INFO>)

    while True:
        try:
            WebDriverWait(browser, 30).until(EC.presence_of_element_located((
                By.XPATH, '//a[@href="<CONFIDENTIAL INFO>"]'
            )))
            break
        except TimeoutException:
            browser.refresh()
            pass

    logged = False

    try:
        logInButton = browser.find_element_by_id('sch_login_lnk')
    except NoSuchElementException:
        logged = True

    if not logged:
        logInButton.click()
        username = browser.find_element_by_xpath("//input[@name='username']")
        password = browser.find_element_by_xpath("//input[@name='password']")

        username.send_keys(<CONFIDENTIAL INFO>)
        password.send_keys(<CONFIDENTIAL INFO>)

        username.submit()

        WebDriverWait(browser, 30).until(EC.url_to_be(
            '<CONFIDENTIAL INFO>/journals'
        ))





def parseSchedule(html):

    schedule = []
    
    soup = BeautifulSoup(html, 'lxml')
    
    lcol = soup.findAll('div', class_ = 'db_days_column left')[0].findAll('div', class_ = 'db_day')
    rcol = soup.findAll('div', class_ = 'db_days_column right')[0].findAll('div', class_ = 'db_day')
    

    for i in range(3):
        schedule.append([])

        day = lcol[i].table.tbody.findAll('tr')

        if len(day) < 7:
            return None

        for lesson in day:
            try:
                name = lesson.findAll('span')[0].text[2:].strip()
            except IndexError:
                name = ''
            
            try:
                hometask = lesson.findAll('div', class_ = 'ht-text')[0].text.strip()
            except IndexError:
                hometask = ''

            try:
                mark = lesson.findAll('strong')[0].text.strip()
            except IndexError:
                mark = ''

            schedule[i].append({"name" : name, "hometask" : hometask, "mark" : mark})

        while schedule[i][-1]["name"] == '':
            del schedule[i][-1]



            

    for i in range(3):
        schedule.append([])

        day = rcol[i].table.tbody.findAll('tr')

        for lesson in day:
            try:
                name = lesson.findAll('span')[0].text[2:].strip()
            except IndexError:
                name = ''
            
            try:
                hometask = lesson.findAll('div', class_ = 'ht-text')[0].text.strip()
            except IndexError:
                hometask = ''

            try:
                mark = lesson.findAll('strong')[0].text.strip()
            except IndexError:
                mark = ''

            schedule[i + 3].append({"name" : name, "hometask" : hometask, "mark" : mark})

        if i != 2:
            while len(schedule[i + 3]) > 0 and schedule[i + 3][-1]["name"] == '':
                del schedule[i + 3][-1]

    schedule.append([])
    for i in range(7):
        schedule[6].append({"name" : "", "hometask" : "", "mark" : ""})

    return schedule
        




def getWeekSchedule(browser, pupil, week = 1):

    browser.get(<CONFIDENTIAL INFO>)
    
    diaryURL = '<CONFIDENTIAL INFO>/pupil/' + pupil + '#dnevnik'

    browser.get(diaryURL)
    try:
        WebDriverWait(browser, 30).until(EC.presence_of_element_located((
            By.XPATH, '//div[@class="db_days clearfix"]'
        )))
    except TimeoutException:
        return -2

    if week == 0:
        try:
            arrow = browser.find_element_by_xpath('//div[@class="db_week"]/div/p/a[@class="prev"]')
            arrow.click()
            WebDriverWait(browser, 30).until(EC.presence_of_element_located((
                By.XPATH, '//div[@style="display: none;"]/div/p/a[@class="prev"]'
            )))

            scheduleHTML = browser.find_element_by_xpath('//div[not(@style="display: none;")]/div[@class="db_days clearfix"]')
        except NoSuchElementException:
            return -1

    elif week == 2:
        try:
            arrow = browser.find_element_by_xpath('//div[@class="db_week"]/div/p/a[@class="next"]')
            arrow.click()
            WebDriverWait(browser, 30).until(EC.presence_of_element_located((
                By.XPATH, '//div[@style="display: none;"]/div/p/a[@class="next"]'
            )))
            
            scheduleHTML = browser.find_element_by_xpath('//div[not(@style="display: none;")]/div[@class="db_days clearfix"]')
        except NoSuchElementException:
            return -1
            
    else:
        scheduleHTML = browser.find_element_by_xpath('//div[@class="db_days clearfix"]')
    
    scheduleHTML = scheduleHTML.get_attribute('innerHTML')
    schedule = parseSchedule(scheduleHTML)

    return schedule




def getCurLesson():
    h = int(datetime.now().astimezone(localTimezone).strftime('%H'))
    m = int(datetime.now().astimezone(localTimezone).strftime('%M'))

    if h < int(lessonTime[0][0]) or (h == int(lessonTime[0][0]) and m < int(lessonTime[0][1])):
        return -1

    if h > int(lessonTime[7][2]) or (h == int(lessonTime[7][2]) and m > int(lessonTime[7][3])):
        return 10 ** 9
    
    for i in range(8):
        if ((int(lessonTime[i][0]) == h and int(lessonTime[i][1]) <= m) or
            (int(lessonTime[i][2]) == h and int(lessonTime[i][3]) >= m)):
            return i

    for i in range(7):
        if ((int(lessonTime[i][2]) == h and int(lessonTime[i][3]) < m) or
            (int(lessonTime[i + 1][0]) == h and int(lessonTime[i + 1][1]) > m)):
            return i + 0.5

    raise Error


def _getDaySchedule(day, week, sch):

    if sch == -2:
        return ['Сайт электронного дневника в данный момент недоступен.']

    if sch == -1 and week == 0:
        return ['********ПРОШЛАЯ НЕДЕЛЯ НЕ ВХОДИТ В ЧЕТВЕРТЬ********']
    elif sch == None:
        return ['********РАСПИСАНИЕ ЕЩЁ НЕ СГЕНЕРИРОВАНО********']
    elif sch == -1 and week == 2:
        return ['********СЛЕДУЮЩАЯ НЕДЕЛЯ НЕ ВХОДИТ В ЧЕТВЕРТЬ********']

    isEmpty = True
    today = (datetime.now().astimezone(localTimezone).weekday() == day)
    curLesson = getCurLesson()
    
    ans = []
    if today and curLesson == -1:
        ans.append('********УРОКИ ЕЩЕ НЕ НАЧАЛИСЬ********')
        
    for i in range(len(sch[day])):

        temp = ''
        if today and i == curLesson:
            temp += 'Сейчас идет: '
        
        lesson = sch[day][i]
        temp += str(i + 1) + '. '

        temp += '[<CONFIDENTIAL INFO>|'

        if lesson['name'] != '':
            isEmpty = False
            temp += lesson['name'] + ' '
        else:
            temp += '(Урока нет) '

        temp += ']'
        temp += (lessonTime[i][0] + ':' + lessonTime[i][1] + ' - '
            + lessonTime[i][2] + ':' + lessonTime[i][3] + '\n')
        temp += '&#12288;&#12288;[<CONFIDENTIAL INFO>|Оценка: ]' + lesson['mark'] + '\n'
        temp += '&#12288;&#12288;[<CONFIDENTIAL INFO>|ДЗ: ]' + lesson['hometask']

        ans.append(temp)

        if today and curLesson == i + 0.5:
            temp = 'Сейчас перемена между этими уроками.'
            ans.append(temp)

    if today and curLesson == 10 ** 9:
        ans.append('********УРОКИ УЖЕ ЗАКОНЧИЛИСЬ*********')

    if isEmpty or len(sch[day]) == 0:
        ans = ['********УРОКОВ НЕТ********']

    ans = ['\n'.join(ans)]

    return ans


def getDaySchedule(browser, day, pupil, week = 1):
    sch = getWeekSchedule(browser, pupil, week)
    return _getDaySchedule(day, week, sch)


def getAllDaysSchedule(browser, pupil, week = 1):
    sch = getWeekSchedule(browser, pupil, week)

    if sch == -2:
        return ['Сайт электронного дневника в данный момент недоступен.']

    ans = []
    for day in range(5):    
        thisDay = _getDaySchedule(day, week, sch)[0]
        thisDay = '------------------> ' + weekdays[day].upper() + ' <------------------\n' + thisDay
        ans.append(thisDay)

    return ans


def getTodaysLesson(browser, pupil, les):
    sch = getWeekSchedule(browser, pupil, week = 1)

    if sch == -2:
        return ['Сайт электронного дневника в данный момент недоступен.']

    if sch == None:
        return ['********РАСПИСАНИЕ ЕЩЁ НЕ СГЕНЕРИРОВАНО********']

    today = datetime.now().astimezone(localTimezone).weekday()
    if les == -1:
        return ['********УРОКИ ЕЩЁ НЕ НАЧАЛИСЬ********']
    elif les >= len(sch[today]) - 0.5:
        return ['********ВСЁ********']

    if len(sch[today]) == 0:
        return ['Сегодня уроков нет.']

    if les - math.trunc(les) == 0:
        lesson = sch[today][les]
        temp = str(les + 1) + '. '

        temp += '[<CONFIDENTIAL INFO>|'

        if lesson['name'] != '':
            temp += lesson['name'] + ' '
        else:
            temp += '(Урока нет) '

        temp += ']'
        temp += (lessonTime[les][0] + ':' + lessonTime[les][1] +
                 ' - ' + lessonTime[les][2] + ':' + lessonTime[les][3] + '\n')
        temp += '&#12288;&#12288;[<CONFIDENTIAL INFO>|Оценка: ]' + lesson['mark'] + '\n'
        temp += '&#12288;&#12288;[<CONFIDENTIAL INFO>|ДЗ: ]' + lesson['hometask']

        return [temp]

    else:
        ans = ['********СЕЙЧАС ПЕРЕМЕНА МЕЖДУ УРОКАМИ********']
        
        lesson = sch[today][math.trunc(les)]
        temp = str(math.trunc(les) + 1) + '. '

        temp += '[<CONFIDENTIAL INFO>|'
        if lesson['name'] != '':
            temp += lesson['name'] + ' '
        else:
            temp += '(Урока нет) '
        temp += ']'
        temp += (lessonTime[math.trunc(les)][0] + ':' + lessonTime[math.trunc(les)][1]
            + ' - ' + lessonTime[math.trunc(les)][2] + ':' + lessonTime[math.trunc(les)][3] + '\n')
        temp += '&#12288;&#12288;[<CONFIDENTIAL INFO>|Оценка: ]' + lesson['mark'] + '\n'
        temp += '&#12288;&#12288;[<CONFIDENTIAL INFO>|ДЗ: ]' + lesson['hometask']

        ans.append(temp)
        ans.append('И')

        lesson = sch[today][math.trunc(les) + 1]

        temp = str(math.trunc(les) + 2) + '. '
        temp += '[<CONFIDENTIAL INFO>|'
        if lesson['name'] != '':
            temp += lesson['name'] + ' '
        else:
            temp += '(Урока нет) '
        temp += ']'
        temp += (lessonTime[math.trunc(les) + 1][0] + ':' + lessonTime[math.trunc(les) + 1][1] +
                 ' - ' + lessonTime[math.trunc(les) + 1][2] + ':' + lessonTime[math.trunc(les) + 1][3] + '\n')
        temp += '&#12288;&#12288;[<CONFIDENTIAL INFO>|Оценка: ]' + lesson['mark'] + '\n'
        temp += '&#12288;&#12288;[<CONFIDENTIAL INFO>|ДЗ: ]' + lesson['hometask']

        ans.append(temp)
        ans = ['\n'.join(ans)]

        return ans
        
    

def getCurrentLesson(browser, pupil):
    return getTodaysLesson(browser, pupil, getCurLesson())

def getNextLesson(browser, pupil):
    return getTodaysLesson(browser, pupil, math.trunc(getCurLesson()) + 1)


def getRings():
    ans = ''
    for i in range(8):
        ans += str(i + 1) + '. '
        ans += (lessonTime[i][0] + ':' + lessonTime[i][1] + ' - '
            + lessonTime[i][2] + ':' + lessonTime[i][3] + '\n')

    return [ans]



def getFormSchedule(browser, form):
    pupil = databaseGet(dataFormId, mutexFormId, form)
    sch = getWeekSchedule(browser, pupil)

    if sch == -2:
        return ['Сайт электронного дневника в данный момент недоступен.']

    if sch == None:
        return ['********РАСПИСАНИЕ ЕЩЁ НЕ СГЕНЕРИРОВАНО********']

    ans = []
    
    for i in range(5):
        temp = weekdays[i] + '\n'
        for j in range(len(sch[i])):
            temp += '&#12288;&#12288;' + str(j + 1) + '. '
            if sch[i][j]['name'] == '':
                temp += '[Урока нет]'
            else:
                temp += sch[i][j]['name']
            temp += '\n'

        ans.append(temp)

    ans = ['\n'.join(ans)]

    return ans



def parseMarks(html):
    soup = BeautifulSoup(html, 'lxml')
    ltable = soup.findAll('table', class_ = 'itable ltable')[0].tbody.findAll('tr')

    subjects = []
    averages = []
    ans = {}

    for subject in ltable:
        subjectName = subject.findAll('td', class_ = 'ttl')[0].text.strip()[3:].strip()
        subjects.append(subjectName)
        averageMark = subject.findAll('span', class_ = 'avg')[0].text
        averages.append(averageMark)

    mtable =  soup.findAll('table', class_ = 'itable mtable')[0].tbody.findAll('tr')

    for i in range(len(subjects)):
        ans[subjects[i]] = {
            "all" : [],
            "average" : averages[i]
        }
        row = mtable[i].findAll('td', class_ = 'lesson_exists')
        ans[subjects[i]]['qmark'] = mtable[i].findAll('td', class_ = 'qmark')[0].text

        for cell in row:
            mark = cell.text.strip()
            if mark.isdigit() or ('/' in mark and mark.split('/')[0].isdigit() and mark.split('/')[1].isdigit()):
                ans[subjects[i]]['all'].append(mark)

            elif '/' in mark and mark.split('/')[0].isdigit():
                ans[subjects[i]]['all'].append(mark.split('/')[0])

            elif '/' in mark and mark.split('/')[1].isdigit():
                ans[subjects[i]]['all'].append(mark.split('/')[1])

    return ans
            
    
    



def getMarks(browser, pupil):
    browser.get(<CONFIDENTIAL INFO>)
    marksURL = <CONFIDENTIAL INFO> + pupil + '#progress'
    browser.get(marksURL)

    try:
        WebDriverWait(browser, 30).until(EC.presence_of_element_located((
            By.XPATH, '//div[@class="wrap_lmtables wider"]'
        )))
    except TimeoutException:
        return ['Сайт электронного дневника в данный момент недоступен.']

    marksHTML = browser.find_element_by_xpath('//div[@class="wrap_lmtables wider"]')
    marksHTML = marksHTML.get_attribute('innerHTML')

    marks = parseMarks(marksHTML)

    ans = []
    for subject in marks:
        temp = '[<CONFIDENTIAL INFO>|' + subject + ': ]'
        for mark in marks[subject]['all']:
            temp += mark + ' '
        temp += '\n&#12288;&#12288;Средняя: ' + marks[subject]['average'] + '\n'

        if len(marks[subject]['qmark']) != 0:
            temp += '&#12288;&#12288;Четвертная: ' + marks[subject]['qmark'] + '\n'

        ans.append(temp)
    ans = ['\n'.join(ans)]

    return ans




def parsePupils(html):
    soup = BeautifulSoup(html, 'lxml')
    pupils = soup.findAll('div', class_ = 'pupil')

    ans = {}

    for pupil in pupils:
        ahref = pupil.findAll('a', class_ = 'user_type_1')[0]
        pupilSurname = ahref.text.split()[0]
        pupilID = ahref['href'][7:]
        ans[pupilSurname] = pupilID
    return ans




def getPupilID(browser, user):

    vkOthMutex.acquire()
    surname = vk_other.users.get(user_ids = user)[0]['last_name']
    vkOthMutex.release()

    for form in forms:
        classID = databaseGet(dataClassId, mutexClassId, form)
        browser.get(<CONFIDENTIAL INFO>)
        classURL = '<CONFIDENTIAL INFO>/class/' + classID + '#pupils'
        browser.get(classURL)
        
        try:
            WebDriverWait(browser, 30).until(EC.presence_of_element_located((
                By.XPATH, '//div[@class="pupils_list"]'
            )))
        except TimeoutException:
            return None

        pupilsListHTML = browser.find_element_by_xpath('//div[@class="pupils_list"]')
        pupilsListHTML = pupilsListHTML.get_attribute('innerHTML')

        pupils = parsePupils(pupilsListHTML)

        for pupil in pupils:
            if pupil == surname:
                return pupils[pupil]

    return None



def sendToMe(user, mes):
    vkOthMutex.acquire()
    name = vk_other.users.get(user_ids = user)[0]['first_name']
    surname = vk_other.users.get(user_ids = user)[0]['last_name']
    vkOthMutex.release()

    myweek = databaseGet(dataWeek, mutexWeek, <CONFIDENTIAL INFO>)
    if myweek == None:
        myweek = 1
    else:
        myweek = int(myweek)
    mymes = name + ' ' + surname + ' ' + str(user) + '\n' + mes
    sendMessage(<CONFIDENTIAL INFO>, mymes, True, myweek)

    


def handle(browser, event_text, event_user_id):

    response = []
    kbd = True
    week = databaseGet(dataWeek, mutexWeek, str(event_user_id))
    if week == None:
        week = 1
    week = int(week)

    pupil = databaseGet(dataPupilId, mutexPupilId, str(event_user_id))    
    if pupil == None:
        databasePut(dataPupilId, mutexPupilId, str(event_user_id), getPupilID(browser, str(event_user_id)))
        pupil = databaseGet(dataPupilId, mutexPupilId, str(event_user_id))
    '''
    if pupil == None:
        response = ["Судя по вашему имени, вы не являетесь учащимся <CONFIDENTIAL INFO>. Обратитесь к администрации группы."]
        
    el'''

    if pupil == None:
        pupil = <CONFIDENTIAL INFO>
    if event_text in forms:
        response = getFormSchedule(browser, event_text)
        
    elif event_text in weekdays:
        response = getDaySchedule(browser, weekdayNoms[event_text], pupil, week)
        
    elif event_text == "сегодня":
        response = getDaySchedule(browser, datetime.now().astimezone(localTimezone).weekday(), pupil, week = 1)
        
    elif event_text == "завтра":
        day = (datetime.now().astimezone(localTimezone).weekday() + 1) % 7
        if day == 0:
            week_ = 2
        else:
            week_ = 1
        response = getDaySchedule(browser, day, pupil, week = week_)
        
    elif event_text == "оценки":
        response = getMarks(browser, pupil)
        
    elif event_text == "текущий урок":
        response = getCurrentLesson(browser, pupil)
        
    elif event_text == "след. урок":
        response = getNextLesson(browser, pupil)
        
    elif event_text == "звонки":
        response = getRings()

    elif event_text == 'прошлая неделя':
        databasePut(dataWeek, mutexWeek, str(event_user_id), '0')
        week = 0
        response = ['Теперь уроки выводятся для прошлой недели.']

    elif event_text == 'эта неделя':
        databasePut(dataWeek, mutexWeek, str(event_user_id), '1')
        week = 1
        response = ['Теперь уроки выводятся для текущей недели.']

    elif event_text == 'след. неделя':
        databasePut(dataWeek, mutexWeek, str(event_user_id), '2')
        week = 2
        response = ['Теперь уроки выводятся для следующей недели.']

    elif event_text == 'помощь' or event_text == 'начать':
        response = ['Здравствуй, дорогой пользователь. Этот бот с помощью ТВОЕЙ ФАМИЛИИ ' +
                    'ВКонтакте получает информацию из ТВОЕГО ЭЛ. ДНЕВНИКА. Если твоя фамилия  вк ' +
                    'и в дневнике не совпадает, сообщи администрации. Ты можешь узнать ДЗ, ' +
                    'РАСПИСАНИЕ, ОЦЕНКИ ПО КАЖДОМУ ПРЕДМЕТУ, СРЕДНИЙ БАЛЛ.\n\n' + 
                    '*****ФУНКЦИИ*****:\n' +
                    '> ТЕКУЩИЙ/СЛЕДУЮЩИЙ УРОК - какой сейчас урок У ТЕБЯ, дз, оценка.\n' +
                    '> СЕГОДНЯ/ЗАВТРА - все уроки, дз, оценки на сегодня или завтра.\n' +
                    '> ПО ДНЯМ НЕДЕЛИ - ВЫБЕРИ НЕДЕЛЮ (прошлая, эта, текущая). Выбранная неделя выделена ЗЕЛЕНЫМ. ' +
                    'Далее нажми на ДЕНЬ НЕДЕЛИ, по которому ты хочешь узнать уроки, дз и оценки, или на НА ВСЮ НЕДЕЛЮ\n' +
                    '> ЗВОНКИ - расписание звонков\n' +
                    '> !!!!ОЦЕНКИ!!!! - теперь ты можешь получить ВСЕ ОЦЕНКИ И СРЕДНИЙ БАЛЛ по каждому предмету по нажатию ОДНОЙ КНОПКИ!\n' +
                    '> ПОМОЩЬ - эта помощь.'
        ]


    elif event_text == 'вся нед.':
        response = getAllDaysSchedule(browser, pupil, week)
        

    if response == []:
        return
    for mes in response:
        sendMessage(user = event_user_id, kbd = kbd, text = mes, week = week)



def _handle(event_text, event_user_id, event_timestamp):

    activeQueriesMutex.acquire()
    dataActiveQueries[event_user_id].add(event_text)
    global numActiveQueries
    numActiveQueries += 1
    activeQueriesMutex.release()

    i = 0
    while True:
        if browserMutexes[i].acquire(False):
            handle(browsers[i], event_text, event_user_id)
            browserMutexes[i].release()
            break
        else:
            i = (i + 1) % BROWSER_NUM
            if i == 0:
                if time.time() - event_timestamp > 10:
                    break
                time.sleep(30)

    activeQueriesMutex.acquire()
    dataActiveQueries[event_user_id].remove(event_text)
    numActiveQueries -= 1
    activeQueriesMutex.release()




dataFormId = {
    <CONFIDENTIAL INFO>
}
mutexFormId = threading.Lock()

dataClassId = {
    <CONFIDENTIAL INFO>
}
mutexClassId = threading.Lock()

dataPupilId = {
    <CONFIDENTIAL INFO>

}
mutexPupilId = threading.Lock()

dataWeek = {}
mutexWeek = threading.Lock()

lessonTime = [
    ['8', '30', '9', '15'],
    ['9', '25', '10', '10'],
    ['10', '25', '11', '10'],
    ['11', '25', '12', '10'],
    ['12', '25', '13', '10'],
    ['13', '25', '14', '10'],
    ['14', '20', '15', '05'],
    ['15', '15', '16', '00']
    #['14', '30', '15', '15'],
    #['15', '30', '16', '15'],
    #['16', '30', '17', '15'],
    #['17', '30', '18', '15'],
    #['18', '30', '19', '15'],
    #['19', '30', '20', '15']
]
weekdays = ['пн', 'вт', 'ср', 'чт', 'пт', 'сб', 'вс']
weekdayNoms = {'пн' : 0, 'вт' : 1, 'ср' : 2, 'чт' : 3, 'пт' : 4, 'сб' : 5, 'вс' : 6}
forms = ['11а', '11б', '11в', '11г', '10а', '10б', '10в', '10г', '10д']



dataActiveQueries = {}
numActiveQueries = 0
activeQueriesMutex = threading.Lock()




localTimezone = timezone('Europe/Minsk')

if not DEBUG:
    token = <CONFIDENTIAL INFO>
else:
    token = <CONFIDENTIAL INFO>


vk_session_main = vk_api.VkApi(token = token)
longpoll = VkLongPoll(vk_session_main)

vk_session_other = vk_api.VkApi(token = token)
vk_other = vk_session_other.get_api()
vkOthMutex = threading.Lock()



BROWSER_NUM = 2

browsers = []
browserMutexes = []

for i in range(BROWSER_NUM):
    if not DEBUG:
        options = selenium.webdriver.chrome.options.Options()
        options.binary_location = '/app/.apt/usr/bin/google-chrome'
        browser = webdriver.Chrome(executable_path = r"/app/.chromedriver/bin/chromedriver", options = options)
    else:
        options = selenium.webdriver.firefox.options.Options()
        #options.headless = True
        browser = webdriver.Firefox(executable_path = r"D:\Downloads\geckodriver-v0.24.0-win64\geckodriver.exe", options = options)

    schoolsByAuthorize(browser)

    browsers.append(browser)
    browserMutexes.append(threading.Lock())


sendMessage(<CONFIDENTIAL INFO>, "Running", True)

for event in longpoll.listen():
    if event.type == VkEventType.MESSAGE_NEW and event.to_me and event.text:

        if time.time() - event.timestamp > 10:
            continue

        week = databaseGet(dataWeek, mutexWeek, str(event.user_id))
        if week == None:
            week = 1
        week = int(week)


        if event.user_id == <CONFIDENTIAL INFO> and event.text.split()[0] == 'wrto':
            who = event.text.split()[1]
            message = ' '.join(event.text.split()[2:])
            whoWeek = databaseGet(dataWeek, mutexWeek, str(who))
            if whoWeek == None:
                whoWeek = 1
            whoWeek = int(whoWeek)

            sendMessage(int(who), message, True, whoWeek)

            continue


        if event.user_id == <CONFIDENTIAL INFO> and event.text.split()[0] == 'all':
            message = ' '.join(event.text.split()[1:])

            allUsers = vk_session_main.method('messages.getConversations', { "count" : 200, "filter" : "all", "offset" : 0 })

            for subscriber in allUsers['items']:
                if subscriber['conversation']['peer']['type'] != 'user':
                    continue

                who = subscriber['conversation']['peer']['id']

                whoWeek = databaseGet(dataWeek, mutexWeek, str(who))
                if whoWeek == None:
                    whoWeek = 1
                whoWeek = int(whoWeek)

                sendMessage(int(who), message, True, whoWeek)




        event.text = event.text.lower()

        if (event.text != 'текущий урок' and event.text != 'след. урок' and event.text != 'сегодня' and
            event.text != 'завтра' and event.text != 'звонки' and event.text != 'начать' and event.text != 'помощь' and
            event.text != 'оценки' and not(event.text in weekdays) and not(event.text in forms) and
            event.text != 'вся нед.' and event.text != 'прошлая неделя' and event.text != 'эта неделя' and
            event.text != 'след. неделя'
            and not(event.user_id == <CONFIDENTIAL INFO> and (event.text == 'stop' or event.text[0:4] == 'wrto'))):

            continue

        elif event.text == "stop" and event.user_id == <CONFIDENTIAL INFO>:
            break

        activeQueriesMutex.acquire()
        try:
            dataActiveQueries[event.user_id]
        except KeyError:
            dataActiveQueries[event.user_id] = set()

        if (numActiveQueries >= 10 or (event.text in dataActiveQueries[event.user_id]) or
            len(dataActiveQueries[event.user_id]) >= 4):

            activeQueriesMutex.release()
            continue

        activeQueriesMutex.release()

        thr = threading.Thread(target = _handle, args = [event.text, event.user_id, event.timestamp])
        thr.daemon = True
        thr.start()


for browser in browsers:
    browser.quit()




