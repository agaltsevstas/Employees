--
-- PostgreSQL database dump
--

-- Dumped from database version 14.2
-- Dumped by pg_dump version 14.2

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: employee; Type: TABLE; Schema: public; Owner: agaltsevstas
--

CREATE TABLE public.employee (
    id integer NOT NULL,
    "position" text NOT NULL,
    surname text NOT NULL,
    name text NOT NULL,
    patronymic text NOT NULL,
    sex character varying(3) NOT NULL,
    dateofbirth date NOT NULL,
    passport bigint NOT NULL,
    phone bigint NOT NULL,
    email text NOT NULL,
    dateofhiring date NOT NULL,
    workinghours text NOT NULL,
    salary money NOT NULL,
    password character varying(128) NOT NULL
);


ALTER TABLE public.employee OWNER TO agaltsevstas;

--
-- Name: employee_id_seq; Type: SEQUENCE; Schema: public; Owner: agaltsevstas
--

CREATE SEQUENCE public.employee_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.employee_id_seq OWNER TO agaltsevstas;

--
-- Name: employee_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: agaltsevstas
--

ALTER SEQUENCE public.employee_id_seq OWNED BY public.employee.id;


--
-- Name: employee id; Type: DEFAULT; Schema: public; Owner: agaltsevstas
--

ALTER TABLE ONLY public.employee ALTER COLUMN id SET DEFAULT nextval('public.employee_id_seq'::regclass);


--
-- Data for Name: employee; Type: TABLE DATA; Schema: public; Owner: agaltsevstas
--

COPY public.employee (id, "position", surname, name, patronymic, sex, dateofbirth, passport, phone, email, dateofhiring, workinghours, salary, password) FROM stdin;
1	Начальник_склада	Волынец	Сергей	Петрович	Муж	1973-05-24	9031526724	5695462581	voljnets.sergej.petrovich@tradingcompany.ru	2018-01-06	Понедельник-Пятница=09:00-18:00	$65,000.00	1Headofwarehouse
0	Начальник_отдела_закупок	Тимофеев	Андрей	Васильевич	Муж	1985-11-23	4951162773	5698423591	timofeev.andrej.vasilevich@tradingcompany.ru	2018-01-09	Понедельник-Пятница=09:00-18:00	$90,000.00	1Headofprocurement
4	Главный_бухгалтер	Смежнова	Ирина	Владимировна	Жен	1986-06-30	9150056750	5371863331	smezhnova.irina.vladimirovna@tradingcompany.ru	2018-01-02	Понедельник-Пятница=09:00-18:00	$100,000.00	1Chiefaccountant
5	Водитель	Смирнов	Илья	Сергеевич	Муж	1991-08-04	9151061883	5958992995	smirnov.ilja.sergeevich@tradingcompany.ru	2018-01-18	Понедельник-Пятница=09:00-12:00	$50,000.00	1Driver
6	Водитель	Васильев	Игорь	Александров	Муж	1970-04-08	9035752330	6968033445	vasilev.igor.aleksandrov@tradingcompany.ru	2018-01-19	Понедельник-Пятница=09:00-12:00	$50,000.00	1dRiver
7	Водитель	Соколов	Адам	Константинович	Муж	1984-10-20	4956152743	5644433805	sokolov.adam.konstantinovich@tradingcompany.ru	2018-01-19	Понедельник-Пятница=09:00-12:00	$50,000.00	1drIver
8	Водитель	Попов	Вадим	Станиславович	Муж	1989-07-07	9161571408	5496472258	popov.vadim.stanislavovich@tradingcompany.ru	2018-01-22	Понедельник-Пятница=12:00-18:00	$50,000.00	1driVer
9	Водитель	Михайлов	Виталий	Игоревич	Муж	1965-05-15	9030061223	5995941090	mihajlov.vitalij.igorevich@tradingcompany.ru	2018-01-22	Понедельник-Пятница=12:00-18:00	$50,000.00	1drivEr
10	Водитель	Кузнецов	Георгий	Мстиславович	Муж	1965-04-04	9156323480	6590067551	kuznetsov.georgij.mstislavovich@tradingcompany.ru	2018-01-25	Понедельник-Пятница=12:00-18:00	$50,000.00	1driveR
11	Водитель	Волков	Михаил	Васильевич	Муж	1975-04-30	4961474345	5005645463	volkov.mihail.vasilevich@tradingcompany.ru	2018-01-25	Понедельник-Пятница=12:00-18:00	$50,000.00	1DRiver
12	Водитель	Новиков	Эдуард	Николаевич	Муж	1976-03-29	9031567337	5548402262	novikov.eduard.nikolaevich@tradingcompany.ru	2018-01-27	Понедельник-Пятница=18:00-21:00	$50,000.00	1DrIver
13	Водитель	Морозов	Юрий	Эдуардович	Муж	1987-12-09	9031567912	5335851781	morozov.jurij.eduardovich@tradingcompany.ru	2018-01-28	Понедельник-Пятница=18:00-21:00	$50,000.00	1DriVer
14	Водитель	Федоров	Сергей	Валентинович	Муж	1965-11-11	9155757739	5465475284	fedorov.sergej.valentinovich@tradingcompany.ru	2018-01-29	Понедельник-Пятница=18:00-21:00	$50,000.00	1DrivEr
15	Водитель	Козлов	Кузьма	Никитич	Муж	1979-08-09	9031663477	5645583005	kozlov.kuzma.nikitich@tradingcompany.ru	2018-01-30	Понедельник-Пятница=18:00-21:00	$50,000.00	1DriveR
16	Грузчик	Петровна	Вероника	Дмитриевна	Жен	1993-04-07	9036558237	5578792245	petrovna.veronika.dmitrievna@tradingcompany.ru	2018-01-15	Понедельник-Пятница=09:00-18:00	$30,000.00	1Stevedore
17	Грузчик	Захарова	Мария	Альбертовна	Жен	1970-12-26	90355727975	5663374173	zaharova.marija.albertovna@tradingcompany.ru	2018-01-16	Понедельник-Пятница=09:00-18:00	$30,000.00	1sTevedore
18	Грузчик	Ермакова	Анна	Богдановна	Жен	1974-06-06	9032456377	554637332	ermakova.anna.bogdanovna@tradingcompany.ru	2018-01-17	Понедельник-Пятница=09:00-18:00	$30,000.00	1stEvedore
19	Грузчик	Никитина	Анастасия	Борисовна	Жен	1983-09-29	915146288	5653412161	nikitina.anastasija.borisovna@tradingcompany.ru	2018-01-18	Понедельник-Пятница=09:00-18:00	$30,000.00	1steVedore
20	Грузчик	Леонова	Валерия	Вячеславовна	Жен	1981-05-19	915176571	5774342527	leonova.valerija.vjacheslavovna@tradingcompany.ru	2018-01-19	Понедельник-Пятница=09:00-18:00	$30,000.00	1stevEdore
21	Грузчик	Смойлова	Яна	Германовна	Жен	1991-11-11	905704034	5995266560	smojlova.jana.germanovna@tradingcompany.ru	2018-01-20	Понедельник-Пятница=09:00-18:00	$30,000.00	1steveDore
22	Грузчик	Дема	Кристина	Борисовна	Жен	1985-01-10	903662470	5605801708	dema.kristina.borisovna@tradingcompany.ru	2018-01-21	Понедельник-Пятница=09:00-18:00	$30,000.00	1stevedOre
23	Грузчик	Нефедова	Эмилия	Гордеевна	Жен	1988-02-03	915156643	5007466232	nefedova.emilija.gordeevna@tradingcompany.ru	2018-01-22	Понедельник-Пятница=09:00-18:00	$30,000.00	1stevedoRe
24	Грузчик	Смактуновский	Любовь	Герасимовна	Жен	1971-11-08	903176486	5065364170	smaktunovskij.ljubov.gerasimovna@tradingcompany.ru	2018-01-23	Понедельник-Пятница=09:00-18:00	$30,000.00	1stevedorEs
25	Грузчик	Хорина	Надежда	Семеновна	Жен	1990-06-21	915047454	5467534886	horina.nadezhda.semenovna@tradingcompany.ru	2018-01-24	Понедельник-Пятница=09:00-18:00	$30,000.00	1stevedorE
26	Юрист	Саркази	Николя	Николкин	Муж	1955-12-13	6031562741	8658623792	sarkazi.nikolja.nikolkin@tradingcompany.ru	2018-01-08	Понедельник-Пятница=09:00-18:00	$100,000.00	1Lawyer
27	Бухгалтер	Волкова	Вероника	Сергеевна	Жен	1994-03-02	9031051780	5658792941	volkova.veronika.sergeyevna@tradingcompany.ru	2018-01-21	Понедельник-Пятница=09:00-18:00	$85,000.00	1Accountant
28	Бухгалтер	Шишкин	Алексей	Александров	Муж	1980-10-27	9035552337	5768433549	shishkin.aleksey.aleksandrov@tradingcompany.ru	2018-01-22	Понедельник-Пятница=09:00-18:00	$80,000.00	1aCcountant
29	Бухгалтер	Фомин	Николай	Алексеевич	Муж	1985-11-23	4951152723	5694423001	fomin.nikolay.alekseyevich@tradingcompany.ru	2018-01-23	Понедельник-Пятница=09:00-18:00	$50,000.00	1acCountant
30	Бухгалтер	Алешин	Вадим	Станиславович	Муж	1981-01-30	9151561826	5596462288	aleshin.vadim.stanislavovich@tradingcompany.ru	2018-01-24	Понедельник-Пятница=09:00-18:00	$80,000.00	1accOuntant
31	Бухгалтер	Томей	Мариса	Игоревна	Жен	1965-05-15	9031661771	5675841595	tomey.marisa.igorevna@tradingcompany.ru	2018-01-25	Понедельник-Пятница=09:00-18:00	$50,000.00	1accoUntant
2	Начальник_склада	Михайлов	Владимир	Никитич	Муж	1969-08-04	9031562761	5635861525	mihajlov.vladimir.nikitich@tradingcompany.ru	2018-01-12	Понедельник-Пятница=09:00-18:00	$60,000.00	1hEadofwarehouse
3	Кассир	Шумихина	Эмилия	Васильевна	Жен	1993-12-01	9153366503	5058742950	shumihina.emilija.vasilevna@tradingcompany.ru	2018-01-23	Понедельник-Пятница=09:00-18:00	$40,000.00	1Cashier
35	Бухгалтер	Михайлов	Михаил	Михайлович	Муж	1977-03-07	9031466862	5635831821	mikhaylov.mikhail.mikhaylovich@tradingcompany.ru	2018-01-29	Понедельник-Пятница=09:00-18:00	$75,000.00	1accountaNt
36	Бухгалтер	Никулин	Сергей	Иванович	Муж	1989-04-22	9035777727	5665445229	nikulin.sergey.ivanovich@tradingcompany.ru	2018-01-30	Понедельник-Пятница=09:00-18:00	$75,000.00	1accountanT
37	Бухгалтер	Михайлов	Владимир	Никитич	Муж	1969-08-04	9031863710	5645681555	mikhaylov.vladimir.nikitich@tradingcompany.ru	2018-01-31	Понедельник-Пятница=09:00-18:00	$75,000.00	1AccountanT
38	Главный_юрист-консультант	Тимошин	Виктор	Васильевич	Муж	1991-06-13	9151156783	5491443561	timoshin.viktor.vasilevich@tradingcompany.ru	2018-01-10	Понедельник-Пятница=09:00-18:00	$80,000.00	1Chieflegalcounsel
39	Директор	Агальцев	Станислав	Сергеевич	Муж	1995-12-16	9032697963	4516560001	stas.agaltsev.sergeevich@tradingcompany.ru	2018-01-01	Понедельник-Пятница=09:00-18:00	$200,000.00	1Director
40	Директор	Фамилия	Имя	Отчество	Муж	2001-01-01	9035678826	4516560002	familija.imja.otchestvo@tradingcompany.ru	2018-01-01	Понедельник-Пятница=09:00-18:00	$200,000.00	2Director
41	Менеджер_по_продажам	Макарик	Анрей	Михайлович	Муж	1984-04-14	9035566749	5655642287	makarik.anrej.mihajlovich@tradingcompany.ru	2018-01-21	Понедельник-Пятница=09:00-18:00	$95,000.00	1Salesmanager
42	Менеджер_по_продажам	Агафонова	Александра	Тимофеевна	Жен	1994-04-14	9036562721	5555643037	agafonova.aleksandra.timofeevna@tradingcompany.ru	2018-01-21	Понедельник-Пятница=09:00-18:00	$90,000.00	1sAlesmanager
43	Менеджер_по_продажам	Коваленко	Нина	Олеговна	Жен	1983-09-22	9153567772	5353632387	kovalenko.nina.olegovna@tradingcompany.ru	2018-01-23	Понедельник-Пятница=09:00-18:00	$90,000.00	1saLesmanager
44	Логист	Волкова	Александра	Александрова	Жен	1982-03-01	9151060703	5658792991	volkova.aleksandra.aleksandrova@tradingcompany.ru	2018-01-11	Понедельник-Пятница=09:00-18:00	$70,000.00	1Logistician
45	Логист	Андропов	Никита	Данилович	Муж	1980-10-27	9031162337	5728403541	andropov.nikita.danilovich@tradingcompany.ru	2018-01-19	Понедельник-Пятница=09:00-18:00	$60,000.00	1lOgistician
46	Логист	Радионов	Дмитрий	Олегович	Муж	1985-11-23	9707821586	9707821586	radionov.dmitrij.olegovich@tradingcompany.ru	2018-01-20	Понедельник-Пятница=09:00-18:00	$55,000.00	1loGistician
47	Менеджер_по_персоналу	Анисимова	Мария	Борисовна	Жен	1990-01-12	9032661077	8810024816	anisimova.marija.borisovna@tradingcompany.ru	2020-04-01	Понедельник-Пятница=09:00-18:00	$70,000.00	1HRmanager
48	Менеджер_по_закупкам	Медведева	Ирина	Владимировна	Жен	1995-05-09	9031558281	5678732543	medvedeva.irina.vladimirovna@tradingcompany.ru	2018-01-15	Понедельник-Пятница=09:00-18:00	$80,000.00	1Purchasingmanager
49	Менеджер_по_закупкам	Дроздов	Даниил	Романович	Муж	1980-10-27	9155526925	5769933123	drozdov.daniil.romanovich@tradingcompany.ru	2018-01-16	Понедельник-Пятница=09:00-18:00	$70,000.00	1pUrchasingmanager
50	Менеджер_по_закупкам	Дали	Сальвадор	Даниилович	Муж	1966-04-02	4952456322	6654433399	dali.salvador.daniilovich@tradingcompany.ru	2018-01-17	Понедельник-Пятница=09:00-18:00	$70,000.00	1puRchasingmanager
51	Менеджер_по_закупкам	Захватошина	Ирина	Генадьевна	Жен	1987-12-31	9151763870	5556462268	zahvatoshina.irina.genadevna@tradingcompany.ru	2018-01-18	Понедельник-Пятница=09:00-18:00	$70,000.00	1purChasingmanager
52	Менеджер_по_закупкам	Селюто	Наталия	Ивановна	Жен	1995-01-17	8451663479	5674846571	seljuto.natalija.ivanovna@tradingcompany.ru	2018-01-19	Понедельник-Пятница=09:00-18:00	$70,000.00	1purcHasingmanager
53	Менеджер_по_закупкам	Высоцкий	Владимир	Андреевич	Муж	1977-10-10	8667341355	6795427560	vjsotskij.vladimir.andreevich@tradingcompany.ru	2018-01-20	Понедельник-Пятница=09:00-18:00	$70,000.00	1purchAsingmanager
54	Менеджер_по_закупкам	Смирнова	Юлия	Викторовна	Жен	1988-08-09	9031622762	5655831728	smirnova.julija.viktorovna@tradingcompany.ru	2018-01-21	Понедельник-Пятница=09:00-18:00	$65,000.00	1purchaSingmanagers
55	Менеджер_по_закупкам	Даль	Олег	Олегович	Муж	1967-07-02	4951566322	6977462212	dal.oleg.olegovich@tradingcompany.ru	2018-01-22	Понедельник-Пятница=09:00-18:00	$65,000.00	1purchasIngmanager
56	Менеджер_по_закупкам	Смактуновский	Инокентий	Арадьевич	Муж	1971-03-03	9031466863	5665337191	smaktunovskij.inokentij.aradevich@tradingcompany.ru	2018-01-23	Понедельник-Пятница=09:00-18:00	$60,000.00	1purchasiNgmanagers
57	Менеджер_по_закупкам	Хепберн	Одри	Антоновна	Жен	1990-09-20	9155474874	5465348261	hepbern.odri.antonovna@tradingcompany.ru	2018-01-24	Понедельник-Пятница=09:00-18:00	$60,000.00	1purchasinGmanager
58	Менеджер_по_закупкам	Терехова	Маргарита	Васильевна	Жен	1992-06-04	9031863332	5645657855	terehova.margarita.vasilevna@tradingcompany.ru	2018-01-25	Понедельник-Пятница=09:00-18:00	$60,000.00	1purchasingManager
32	Бухгалтер	Сталлоне	Сильвестр	Сталовович	Муж	1955-11-09	9151321785	6694467581	ctallone.cilvestr.ctalovovich@tradingcompany.ru	2018-01-26	Понедельник-Пятница=09:00-18:00	$80,000.00	1accouNtant
33	Бухгалтер	Антоновна	Лидия	Васильевна	Жен	1955-12-07	4951572363	6665841221	antonovna.lidiya.vasilyevna@tradingcompany.ru	2018-01-27	Понедельник-Пятница=09:00-18:00	$50,000.00	1accounTant
34	Бухгалтер	Сигал	Стивен	Николаевич	Муж	1956-11-29	4951566384	6947462282	sigal.stiven.nikolayevich@tradingcompany.ru	2018-01-28	Понедельник-Пятница=09:00-18:00	$50,000.00	1accountAnt
\.


--
-- Name: employee_id_seq; Type: SEQUENCE SET; Schema: public; Owner: agaltsevstas
--

SELECT pg_catalog.setval('public.employee_id_seq', 300, true);


--
-- Name: employee employee_pkey; Type: CONSTRAINT; Schema: public; Owner: agaltsevstas
--

ALTER TABLE ONLY public.employee
    ADD CONSTRAINT employee_pkey PRIMARY KEY (id);


--
-- PostgreSQL database dump complete
--

