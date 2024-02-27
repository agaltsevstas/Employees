--
-- PostgreSQL database dump
--

-- Dumped from database version 14.2
-- Dumped by pg_dump version 16.2

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

--
-- Name: public; Type: SCHEMA; Schema: -; Owner: agaltsevstas
--

-- *not* creating schema, since initdb creates it


ALTER SCHEMA public OWNER TO agaltsevstas;

--
-- Name: action_type; Type: TYPE; Schema: public; Owner: agaltsevstas
--

CREATE TYPE public.action_type AS ENUM (
    'lock',
    'read',
    'write'
);


ALTER TYPE public.action_type OWNER TO agaltsevstas;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: database_permission; Type: TABLE; Schema: public; Owner: agaltsevstas
--

CREATE TABLE public.database_permission (
    role_id integer NOT NULL,
    role public.action_type NOT NULL,
    surname public.action_type NOT NULL,
    name public.action_type NOT NULL,
    patronymic public.action_type NOT NULL,
    sex public.action_type NOT NULL,
    date_of_birth public.action_type NOT NULL,
    passport public.action_type NOT NULL,
    phone public.action_type NOT NULL,
    email public.action_type NOT NULL,
    date_of_hiring public.action_type NOT NULL,
    working_hours public.action_type NOT NULL,
    salary public.action_type NOT NULL,
    password public.action_type NOT NULL
);


ALTER TABLE public.database_permission OWNER TO agaltsevstas;

--
-- Name: database_permission_role_id_seq; Type: SEQUENCE; Schema: public; Owner: agaltsevstas
--

ALTER TABLE public.database_permission ALTER COLUMN role_id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.database_permission_role_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: employee; Type: TABLE; Schema: public; Owner: agaltsevstas
--

CREATE TABLE public.employee (
    id integer NOT NULL,
    role_id integer NOT NULL,
    surname text NOT NULL,
    name text NOT NULL,
    patronymic text NOT NULL,
    sex character varying(3) NOT NULL,
    date_of_birth date NOT NULL,
    passport bigint NOT NULL,
    phone bigint NOT NULL,
    email text NOT NULL,
    date_of_hiring date NOT NULL,
    working_hours text NOT NULL,
    salary numeric(10,2) NOT NULL,
    password character varying(128) NOT NULL
);


ALTER TABLE public.employee OWNER TO agaltsevstas;

--
-- Name: employee_id_seq; Type: SEQUENCE; Schema: public; Owner: agaltsevstas
--

ALTER TABLE public.employee ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.employee_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: object; Type: TABLE; Schema: public; Owner: agaltsevstas
--

CREATE TABLE public.object (
    id integer NOT NULL,
    code text NOT NULL,
    name text NOT NULL
);


ALTER TABLE public.object OWNER TO agaltsevstas;

--
-- Name: object_id_seq; Type: SEQUENCE; Schema: public; Owner: agaltsevstas
--

ALTER TABLE public.object ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.object_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: permission; Type: TABLE; Schema: public; Owner: agaltsevstas
--

CREATE TABLE public.permission (
    id integer NOT NULL,
    role_id integer NOT NULL,
    show_db boolean NOT NULL,
    change_rights boolean NOT NULL,
    create_user boolean NOT NULL,
    delete_user boolean NOT NULL
);


ALTER TABLE public.permission OWNER TO agaltsevstas;

--
-- Name: permission_id_seq; Type: SEQUENCE; Schema: public; Owner: agaltsevstas
--

ALTER TABLE public.permission ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.permission_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: personal_data_permission; Type: TABLE; Schema: public; Owner: agaltsevstas
--

CREATE TABLE public.personal_data_permission (
    role_id integer NOT NULL,
    role public.action_type NOT NULL,
    surname public.action_type NOT NULL,
    name public.action_type NOT NULL,
    patronymic public.action_type NOT NULL,
    sex public.action_type NOT NULL,
    date_of_birth public.action_type NOT NULL,
    passport public.action_type NOT NULL,
    phone public.action_type NOT NULL,
    email public.action_type NOT NULL,
    date_of_hiring public.action_type NOT NULL,
    working_hours public.action_type NOT NULL,
    salary public.action_type NOT NULL,
    password public.action_type NOT NULL
);


ALTER TABLE public.personal_data_permission OWNER TO agaltsevstas;

--
-- Name: personal_data_permission_role_id_seq; Type: SEQUENCE; Schema: public; Owner: agaltsevstas
--

ALTER TABLE public.personal_data_permission ALTER COLUMN role_id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.personal_data_permission_role_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: role; Type: TABLE; Schema: public; Owner: agaltsevstas
--

CREATE TABLE public.role (
    id integer NOT NULL,
    code text NOT NULL,
    name text NOT NULL
);


ALTER TABLE public.role OWNER TO agaltsevstas;

--
-- Name: role_id_seq; Type: SEQUENCE; Schema: public; Owner: agaltsevstas
--

ALTER TABLE public.role ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.role_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: tmp; Type: TABLE; Schema: public; Owner: agaltsevstas
--

CREATE TABLE public.tmp (
    show_db boolean,
    change_rights boolean,
    create_user boolean,
    delete_user boolean
);


ALTER TABLE public.tmp OWNER TO agaltsevstas;

--
-- Data for Name: database_permission; Type: TABLE DATA; Schema: public; Owner: agaltsevstas
--

COPY public.database_permission (role_id, role, surname, name, patronymic, sex, date_of_birth, passport, phone, email, date_of_hiring, working_hours, salary, password) FROM stdin;
0	write	write	write	write	write	write	write	write	write	write	write	write	write
1	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock
2	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock
3	read	read	read	read	read	read	read	read	read	read	read	read	read
4	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock
5	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock
6	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock
7	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock
8	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock
9	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock
10	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock
11	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock
12	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock
13	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock	lock
\.


--
-- Data for Name: employee; Type: TABLE DATA; Schema: public; Owner: agaltsevstas
--

COPY public.employee (id, role_id, surname, name, patronymic, sex, date_of_birth, passport, phone, email, date_of_hiring, working_hours, salary, password) FROM stdin;
8	13	Попов	Вадим	Станиславович	Муж	1989-07-07	9161571408	5496472258	popov.vadim.stanislavovich@tradingcompany.ru	2018-01-22	Понедельник-Пятница=12:00-18:00	50000.00	238136e1e12ed7bf6561af20af5e22df
16	12	Петровна	Вероника	Дмитриевна	Жен	1993-04-07	9036558237	5578792245	petrovna.veronika.dmitrievna@tradingcompany.ru	2018-01-15	Понедельник-Пятница=09:00-18:00	30000.00	80e3492a3339f6b3ea501cab6400d32c
26	10	Саркази	Николя	Николкин	Муж	1955-12-13	6031562741	8658623792	sarkazi.nikolja.nikolkin@tradingcompany.ru	2018-01-08	Понедельник-Пятница=09:00-18:00	100000.00	183a580687db7586394270e588bd655c
27	8	Волкова	Вероника	Сергеевна	Жен	1994-03-02	9031051780	5658792941	volkova.veronika.sergeyevna@tradingcompany.ru	2018-01-21	Понедельник-Пятница=09:00-18:00	85000.00	1059c1579a8b95713ef3918ee6ff071e
28	8	Шишкин	Алексей	Александров	Муж	1980-10-27	9035552337	5768433549	shishkin.aleksey.aleksandrov@tradingcompany.ru	2018-01-22	Понедельник-Пятница=09:00-18:00	80000.00	b224eb80df02c9dcbb87123b97bfff4b
29	8	Фомин	Николай	Алексеевич	Муж	1985-11-23	4951152723	5694423001	fomin.nikolay.alekseyevich@tradingcompany.ru	2018-01-23	Понедельник-Пятница=09:00-18:00	50000.00	1c9d0b4f757b12dc0cb729e7528140da
30	8	Алешин	Вадим	Станиславович	Муж	1981-01-30	9151561826	5596462288	aleshin.vadim.stanislavovich@tradingcompany.ru	2018-01-24	Понедельник-Пятница=09:00-18:00	80000.00	736c8acbc7bdc6872eaaaa5e31bdb326
31	8	Томей	Мариса	Игоревна	Жен	1965-05-15	9031661771	5675841595	tomey.marisa.igorevna@tradingcompany.ru	2018-01-25	Понедельник-Пятница=09:00-18:00	50000.00	3abc8debc519859a1b515ab05c6890c2
3	7	Шумихина	Эмилия	Васильевна	Жен	1993-12-01	9153366503	5058742950	shumihina.emilija.vasilevna@tradingcompany.ru	2018-01-23	Понедельник-Пятница=09:00-18:00	40000.00	8049de90881d92faaa835b4f42253e90
35	8	Михайлов	Михаил	Михайлович	Муж	1977-03-07	9031466862	5635831821	mikhaylov.mikhail.mikhaylovich@tradingcompany.ru	2018-01-29	Понедельник-Пятница=09:00-18:00	75000.00	97a5ccd0e7ded3ff3b52f6c0345a3d47
6	13	Васильев	Игорь	Александров	Муж	1970-04-08	9999999999	6968033445	vasilev.igor.aleksandrov@tradingcompany.ru	2018-01-19	Понедельник-Пятница=09:00-12:00	50000.00	67a80bfb75fdabb155ef375f4ed47d56
17	12	Захарова	Мария	Альбертовна	Жен	1970-12-26	9035572797	5663374173	zaharova.marija.albertovna@tradingcompany.ru	2018-01-16	Понедельник-Пятница=09:00-18:00	30000.00	3763e0a87566c01f4134dc52455ed9aa
19	12	Никитина	Анастасия	Борисовна	Жен	1983-09-29	9151462885	5653412161	nikitina.anastasija.borisovna@tradingcompany.ru	2018-01-18	Понедельник-Пятница=09:00-18:00	30000.00	1635efec12156b2a9cf4de9e399f6776
13	13	Морозов	Юрийй	Эдуардович	Муж	1987-12-04	9031567912	5335851781	morozov.jurij.eduardovich@tradingcompany.ru	2018-01-28	Понедельник-Пятница=18:00-21:00	50000.00	c6c0516c36140d62b9adfaf9493692a6
2	5	Михайлов	Владимир	Никитич	Муж	1969-08-04	9031562761	5635861525	mihajlov.vladimir.nikitich@tradingcompany.ru	2018-01-12	Понедельник-Пятница=09:00-18:00	60000.00	d2688e887455c0efabddf5005273d3b3
4	1	Смежнова	Ирина	Владимировна	Жен	1986-06-30	9150056750	5371863331	smezhnova.irina.vladimirovna@tradingcompany.ru	2018-01-02	Понедельник-Пятница=09:00-18:00	100000.00	58fd95fa769899f427308cef14ebab2c
20	12	Леонова	Валерия	Вячеславовна	Жен	1981-05-19	9151765715	5774342527	leonova.valerija.vjacheslavovna@tradingcompany.ru	2018-01-19	Понедельник-Пятница=09:00-18:00	30000.00	d6e039bff9703377a61c896ab2ce5217
21	12	Смойлова	Яна	Германовна	Жен	1991-11-11	9057040345	5995266560	smojlova.jana.germanovna@tradingcompany.ru	2018-01-20	Понедельник-Пятница=09:00-18:00	30000.00	fbffe4e834f0168a8272bf0d57e68ba8
22	12	Дема	Кристина	Борисовна	Жен	1985-01-10	9036624705	5605801708	dema.kristina.borisovna@tradingcompany.ru	2018-01-21	Понедельник-Пятница=09:00-18:00	30000.00	aeabd625f8c15a1bc36c60bd78cc556c
23	12	Нефедова	Эмилия	Гордеевна	Жен	1988-02-03	9151566435	5007466232	nefedova.emilija.gordeevna@tradingcompany.ru	2018-01-22	Понедельник-Пятница=09:00-18:00	30000.00	90d6c504863e85e88d641878e24b7e82
24	12	Смактуновский	Любовь	Герасимовна	Жен	1971-11-08	9031764865	5065364170	smaktunovskij.ljubov.gerasimovna@tradingcompany.ru	2018-01-23	Понедельник-Пятница=09:00-18:00	30000.00	29a93a1ba60676ecd35d1b984b4d23c7
25	12	Хорина	Надежда	Семеновна	Жен	1990-06-21	9150474545	5467534886	horina.nadezhda.semenovna@tradingcompany.ru	2018-01-24	Понедельник-Пятница=09:00-18:00	30000.00	4f45694261b0e7927cde5a2b2c712cb9
18	12	Ермакова	Анна	Богдановна	Жен	1974-06-06	5546373323	5546373323	ermakova.anna.bogdanovna@tradingcompany.ru	2018-01-17	Понедельник-Пятница=09:00-18:00	30000.00	81bc6361e3c9a3e3bc40561b4083116c
15	12	Козлов	Кузьма	Никитич	Муж	1979-08-09	9031663477	5645583005	kozlov.kuzma.nikitich@tradingcompany.ru	2018-01-30	Понедельник-Пятница=18:00-21:00	50000.00	a6d20e1c2b552312bf1d82ae114f7a36
14	13	Федоров	Сергей	Валентинович	Муж	1965-11-11	9155757734	5465475285	fedorov.sergej.valentinovich@tradingcompany.ru	2018-01-29	Понедельник-Пятница=18:00-21:00	50000.00	c09a3fc1402212582e6c7d383fdb0f25
7	13	Соколов	Адам	Константинович	Муж	1984-10-20	4956152743	5644433805	sokolov.adam.konstantinovich@tradingcompany.ru	2018-01-19	Понедельник-Пятница=09:00-12:00	50000.00	477d62fb22d61c29a9ea0fefa5b51465
11	13	Волков	Михаил	Васильевич	Муж	1975-04-30	4961474345	5005645463	volkov.mihail.vasilevich@tradingcompany.ru	2018-01-25	Понедельник-Пятница=12:00-18:00	50000.00	569b327e32ef57bea59e68cf752fa3ab
12	13	Новиков	Эдуард	Николаевич	Муж	1976-03-29	9031567335	5548402262	novikov.eduard.nikolaevich@tradingcompany.ru	2018-01-27	Понедельник-Пятница=18:00-21:00	50000.00	a2a9766b2511ca521e468ea5dc915ddc
1	12	Волынец	Сергей	Петрович	Муж	1973-05-24	9031526724	5695462581	voljnets.sergej.petrovich@tradingcompany.ru	2018-01-06	Понедельник-Пятница=09:00-18:00	65000.00	b23888a58a305cb9ec0854e213755462
36	8	Никулин	Сергей	Иванович	Муж	1989-04-22	9035777727	5665445229	nikulin.sergey.ivanovich@tradingcompany.ru	2018-01-30	Понедельник-Пятница=09:00-18:00	75000.00	ad16f17252bd079c1a0051f9fd01a47a
37	8	Михайлов	Владимир	Никитич	Муж	1969-08-04	9031863710	5645681555	mikhaylov.vladimir.nikitich@tradingcompany.ru	2018-01-31	Понедельник-Пятница=09:00-18:00	75000.00	450d4d7db2c76d2ae665c36089adb32e
38	2	Тимошин	Виктор	Васильевич	Муж	1991-06-13	9151156783	5491443561	timoshin.viktor.vasilevich@tradingcompany.ru	2018-01-10	Понедельник-Пятница=09:00-18:00	80000.00	cd35bb65ffde081d9fb83f1de24b32e8
10	13	Кузнецов	Георгий	Мстиславович	Муж	1965-04-04	9156323480	6590067551	kuznetsov.georgij.mstislavovich@tradingcompany.ru	2018-01-25	Понедельник-Пятница=12:00-18:00	50000.00	a45a6887b8bd87095ffa231461b90548
9	13	Михайлов	Виталий	Игоревич	Муж	1965-05-15	9030061223	5995941090	mihajlov.vitalij.igorevich@tradingcompany.ru	2018-01-22	Понедельник-Пятница=12:00-18:00	50000.00	8a324a58dc3c60e06fe86e818a25eca2
40	0	Фамилия	Имя	Отчество	Муж	2001-01-01	9035678826	4516560002	familija.imja.otchestvo@tradingcompany.ru	2018-01-01	Понедельник-Пятница=09:00-18:00	200000.00	32f0190272bfb3a4550a93fcc906b040
41	6	Макарик	Анрей	Михайлович	Муж	1984-04-14	9035566749	5655642287	makarik.anrej.mihajlovich@tradingcompany.ru	2018-01-21	Понедельник-Пятница=09:00-18:00	95000.00	3fd0c8d8f4d57a5a2125104b5f86365a
42	6	Агафонова	Александра	Тимофеевна	Жен	1994-04-14	9036562721	5555643037	agafonova.aleksandra.timofeevna@tradingcompany.ru	2018-01-21	Понедельник-Пятница=09:00-18:00	90000.00	cfaa0b1509061ffb94a3c26b0f72c07c
43	6	Коваленко	Нина	Олеговна	Жен	1983-09-22	9153567772	5353632387	kovalenko.nina.olegovna@tradingcompany.ru	2018-01-23	Понедельник-Пятница=09:00-18:00	90000.00	f0181576b43bd84746ee39d2f1384c26
44	9	Волкова	Александра	Александрова	Жен	1982-03-01	9151060703	5658792991	volkova.aleksandra.aleksandrova@tradingcompany.ru	2018-01-11	Понедельник-Пятница=09:00-18:00	70000.00	92daa78dd0fbc004bc10b118e56049e5
45	9	Андропов	Никита	Данилович	Муж	1980-10-27	9031162337	5728403541	andropov.nikita.danilovich@tradingcompany.ru	2018-01-19	Понедельник-Пятница=09:00-18:00	60000.00	eac63b240a692a8864c37e8771db5c41
46	9	Радионов	Дмитрий	Олегович	Муж	1985-11-23	9707821586	9707821586	radionov.dmitrij.olegovich@tradingcompany.ru	2018-01-20	Понедельник-Пятница=09:00-18:00	55000.00	661184bc0f4259c2fe7d5d0e275de98d
48	11	Медведева	Ирина	Владимировна	Жен	1995-05-09	9031558281	5678732543	medvedeva.irina.vladimirovna@tradingcompany.ru	2018-01-15	Понедельник-Пятница=09:00-18:00	80000.00	29ce02c8ef1c1e07bf5c9219216230fb
49	11	Дроздов	Даниил	Романович	Муж	1980-10-27	9155526925	5769933123	drozdov.daniil.romanovich@tradingcompany.ru	2018-01-16	Понедельник-Пятница=09:00-18:00	70000.00	1a574b41f2d6844042fb659e0a1ac0f4
50	11	Дали	Сальвадор	Даниилович	Муж	1966-04-02	4952456322	6654433399	dali.salvador.daniilovich@tradingcompany.ru	2018-01-17	Понедельник-Пятница=09:00-18:00	70000.00	b888e4ab4f00e098e34cf78a64a2efb3
51	11	Захватошина	Ирина	Генадьевна	Жен	1987-12-31	9151763870	5556462268	zahvatoshina.irina.genadevna@tradingcompany.ru	2018-01-18	Понедельник-Пятница=09:00-18:00	70000.00	d0efb68269f3953437eaa996b3e2102b
52	11	Селюто	Наталия	Ивановна	Жен	1995-01-17	8451663479	5674846571	seljuto.natalija.ivanovna@tradingcompany.ru	2018-01-19	Понедельник-Пятница=09:00-18:00	70000.00	0bb047a67f149c9dde8d84cd7d9d6b63
53	11	Высоцкий	Владимир	Андреевич	Муж	1977-10-10	8667341355	6795427560	vjsotskij.vladimir.andreevich@tradingcompany.ru	2018-01-20	Понедельник-Пятница=09:00-18:00	70000.00	5cd0ebe7a016cbc6d644e005acfd2509
54	11	Смирнова	Юлия	Викторовна	Жен	1988-08-09	9031622762	5655831728	smirnova.julija.viktorovna@tradingcompany.ru	2018-01-21	Понедельник-Пятница=09:00-18:00	65000.00	649082c6c49c9d74938158fda46c498b
55	11	Даль	Олег	Олегович	Муж	1967-07-02	4951566322	6977462212	dal.oleg.olegovich@tradingcompany.ru	2018-01-22	Понедельник-Пятница=09:00-18:00	65000.00	6a373f303c85fe4630a162a04552dcb1
56	11	Смактуновский	Инокентий	Арадьевич	Муж	1971-03-03	9031466863	5665337191	smaktunovskij.inokentij.aradevich@tradingcompany.ru	2018-01-23	Понедельник-Пятница=09:00-18:00	60000.00	c3ca94b01e4813095757a11e6f57701e
57	11	Хепберн	Одри	Антоновна	Жен	1990-09-20	9155474874	5465348261	hepbern.odri.antonovna@tradingcompany.ru	2018-01-24	Понедельник-Пятница=09:00-18:00	60000.00	00bdfc5998ddeb09226020fd3b7ad493
32	8	Сталлоне	Сильвестр	Сталовович	Муж	1955-11-09	9151321785	6694467581	ctallone.cilvestr.ctalovovich@tradingcompany.ru	2018-01-26	Понедельник-Пятница=09:00-18:00	80000.00	914162a3a9fbf0003dc8d7fc90f4d9bb
33	8	Антоновна	Лидия	Васильевна	Жен	1955-12-07	4951572363	6665841221	antonovna.lidiya.vasilyevna@tradingcompany.ru	2018-01-27	Понедельник-Пятница=09:00-18:00	50000.00	62fda6066ce7e81a235f58ab1140084f
34	8	Сигал	Стивен	Николаевич	Муж	1956-11-29	4951566384	6947462282	sigal.stiven.nikolayevich@tradingcompany.ru	2018-01-28	Понедельник-Пятница=09:00-18:00	50000.00	2d997a963f6c55e49d65dba219788015
58	11	Терехова	Маргарита	Васильевна	Жен	1992-06-04	9031863332	5645657855	terehova.margarita.vasilevna@tradingcompany.ru	2018-01-25	Понедельник-Пятница=09:00-18:00	60000.00	cdcac094bce1e6afa125846b5e20b465
5	8	Смирнов	Илья	Сергеевич	Муж	1991-08-04	9151061883	5958992995	smirnov.ilja.sergeevich@tradingcompany.ru	2018-01-18	Понедельник-Пятница=09:00-12:00	50000.00	f6e3ba7fb9b23986a4029dd924c50607
39	0	Агальцев	Станислав	Сергеевич	Муж	1995-12-16	9032697963	4516560001	stas.agaltsev.sergeevich@tradingcompany.ru	2018-01-01	Понедельник-Пятница=09:00-18:00	200000.00	decac1b4b0cac2f363c12e200da9ff07
47	3	Анисимова	Мария	Борисовна	Жен	1990-01-12	9032661077	8810024816	anisimova.marija.borisovna@tradingcompany.ru	2020-04-01	Понедельник-Пятница=09:00-18:00	70000.00	332ac0039d4810e54a377e70493d866b
\.


--
-- Data for Name: object; Type: TABLE DATA; Schema: public; Owner: agaltsevstas
--

COPY public.object (id, code, name) FROM stdin;
0	surname	Фамилия
1	name	Имя
2	patronymic	Отчество
3	sex	Пол
4	date_of_birth	Дата_рождения
5	passport	Паспорт
6	phone	Телефон
7	email	Почта
8	date_of_hiring	Дата_принятия_на_работу
9	working_hours	Часы_работы
10	salary	Зарплата
11	password	Пароль
\.


--
-- Data for Name: permission; Type: TABLE DATA; Schema: public; Owner: agaltsevstas
--

COPY public.permission (id, role_id, show_db, change_rights, create_user, delete_user) FROM stdin;
0	0	t	t	t	t
1	1	f	f	f	f
2	2	f	f	f	f
3	3	t	f	t	f
4	4	f	f	f	f
5	5	f	f	f	f
6	6	f	f	f	f
7	7	f	f	f	f
8	8	f	f	f	f
9	9	f	f	f	f
10	10	f	f	f	f
11	11	f	f	f	f
12	12	f	f	f	f
13	13	f	f	f	f
\.


--
-- Data for Name: personal_data_permission; Type: TABLE DATA; Schema: public; Owner: agaltsevstas
--

COPY public.personal_data_permission (role_id, role, surname, name, patronymic, sex, date_of_birth, passport, phone, email, date_of_hiring, working_hours, salary, password) FROM stdin;
0	write	write	write	write	write	write	write	write	write	write	write	write	write
1	read	read	read	read	read	read	read	write	read	read	read	read	write
2	read	read	read	read	read	read	read	write	read	read	read	read	write
3	read	read	read	read	read	read	read	write	read	read	read	read	write
4	read	read	read	read	read	read	read	write	read	read	read	read	write
5	read	read	read	read	read	read	read	write	read	read	read	read	write
6	read	read	read	read	read	read	read	write	read	read	read	read	write
7	read	read	read	read	read	read	read	write	read	read	read	read	write
8	read	read	read	read	read	read	read	write	read	read	read	read	write
9	read	read	read	read	read	read	read	write	read	read	read	read	write
10	read	read	read	read	read	read	read	write	read	read	read	read	write
11	read	read	read	read	read	read	read	write	read	read	read	read	write
12	read	read	read	read	read	read	read	write	read	read	read	read	write
13	read	read	read	read	read	read	read	write	read	read	read	read	write
\.


--
-- Data for Name: role; Type: TABLE DATA; Schema: public; Owner: agaltsevstas
--

COPY public.role (id, code, name) FROM stdin;
0	director	Директор
3	hr-manager	Менеджер по персоналу
7	сashier	Кассир
8	accountant	Бухгалтер
9	logistician	Логист
10	lawyer	Юрист
12	loader	Грузчик
2	chief_legal_сonsultant	Главный юрист-консультант
4	head_of_procurement	Начальник отдела закупок
5	head_of_warehouse	Начальник склада
6	sales_manager	Менеджер по продажам
11	purchasing_manager	Менеджер по закупкам
1	chief_accountant	Главный бухгалтер
13	driver	Водитель
\.


--
-- Data for Name: tmp; Type: TABLE DATA; Schema: public; Owner: agaltsevstas
--

COPY public.tmp (show_db, change_rights, create_user, delete_user) FROM stdin;
t	t	t	t
\.


--
-- Name: database_permission_role_id_seq; Type: SEQUENCE SET; Schema: public; Owner: agaltsevstas
--

SELECT pg_catalog.setval('public.database_permission_role_id_seq', 1, false);


--
-- Name: employee_id_seq; Type: SEQUENCE SET; Schema: public; Owner: agaltsevstas
--

SELECT pg_catalog.setval('public.employee_id_seq', 4, true);


--
-- Name: object_id_seq; Type: SEQUENCE SET; Schema: public; Owner: agaltsevstas
--

SELECT pg_catalog.setval('public.object_id_seq', 1, false);


--
-- Name: permission_id_seq; Type: SEQUENCE SET; Schema: public; Owner: agaltsevstas
--

SELECT pg_catalog.setval('public.permission_id_seq', 1, false);


--
-- Name: personal_data_permission_role_id_seq; Type: SEQUENCE SET; Schema: public; Owner: agaltsevstas
--

SELECT pg_catalog.setval('public.personal_data_permission_role_id_seq', 1, false);


--
-- Name: role_id_seq; Type: SEQUENCE SET; Schema: public; Owner: agaltsevstas
--

SELECT pg_catalog.setval('public.role_id_seq', 1, false);


--
-- Name: database_permission database_permission_pkey; Type: CONSTRAINT; Schema: public; Owner: agaltsevstas
--

ALTER TABLE ONLY public.database_permission
    ADD CONSTRAINT database_permission_pkey PRIMARY KEY (role_id);


--
-- Name: employee employee_pkey; Type: CONSTRAINT; Schema: public; Owner: agaltsevstas
--

ALTER TABLE ONLY public.employee
    ADD CONSTRAINT employee_pkey PRIMARY KEY (id);


--
-- Name: object object_pkey; Type: CONSTRAINT; Schema: public; Owner: agaltsevstas
--

ALTER TABLE ONLY public.object
    ADD CONSTRAINT object_pkey PRIMARY KEY (id);


--
-- Name: permission permission_pkey1; Type: CONSTRAINT; Schema: public; Owner: agaltsevstas
--

ALTER TABLE ONLY public.permission
    ADD CONSTRAINT permission_pkey1 PRIMARY KEY (id);


--
-- Name: personal_data_permission personal_data_permission_pkey; Type: CONSTRAINT; Schema: public; Owner: agaltsevstas
--

ALTER TABLE ONLY public.personal_data_permission
    ADD CONSTRAINT personal_data_permission_pkey PRIMARY KEY (role_id);


--
-- Name: role role_pkey; Type: CONSTRAINT; Schema: public; Owner: agaltsevstas
--

ALTER TABLE ONLY public.role
    ADD CONSTRAINT role_pkey PRIMARY KEY (id);


--
-- Name: personal_data_permission role_id; Type: FK CONSTRAINT; Schema: public; Owner: agaltsevstas
--

ALTER TABLE ONLY public.personal_data_permission
    ADD CONSTRAINT role_id FOREIGN KEY (role_id) REFERENCES public.role(id) NOT VALID;


--
-- Name: employee role_pkey; Type: FK CONSTRAINT; Schema: public; Owner: agaltsevstas
--

ALTER TABLE ONLY public.employee
    ADD CONSTRAINT role_pkey FOREIGN KEY (role_id) REFERENCES public.role(id);


--
-- Name: permission role_pkey; Type: FK CONSTRAINT; Schema: public; Owner: agaltsevstas
--

ALTER TABLE ONLY public.permission
    ADD CONSTRAINT role_pkey FOREIGN KEY (role_id) REFERENCES public.role(id) NOT VALID;


--
-- Name: database_permission role_pkey; Type: FK CONSTRAINT; Schema: public; Owner: agaltsevstas
--

ALTER TABLE ONLY public.database_permission
    ADD CONSTRAINT role_pkey FOREIGN KEY (role_id) REFERENCES public.role(id) NOT VALID;


--
-- Name: SCHEMA public; Type: ACL; Schema: -; Owner: agaltsevstas
--

REVOKE USAGE ON SCHEMA public FROM PUBLIC;
GRANT ALL ON SCHEMA public TO PUBLIC;


--
-- PostgreSQL database dump complete
--

