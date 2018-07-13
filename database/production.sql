CREATE DATABASE IF NOT EXISTS trading_system;

USE trading_system;

CREATE TABLE IF NOT EXISTS signals
(
  name VARCHAR(50),
  datetime VARCHAR(30),
  value TEXT
);

CREATE TABLE IF NOT EXISTS accounts
(
  account_id VARCHAR(50),
  account TEXT,
  PRIMARY KEY (account_id)
);

CREATE TABLE IF NOT EXISTS snapshots
(
  instrument_id VARCHAR(50),
  exchange_id VARCHAR(50),
  exchange_inst_id VARCHAR(50),
  last_price DOUBLE,
  pre_settlement_price DOUBLE,
  pre_close_price DOUBLE,
  pre_open_interest BIGINT,
  open_price DOUBLE,
  highest_price DOUBLE,
  lowest_price DOUBLE,
  total_volume BIGINT,
  current_volume BIGINT,
  turn_over DOUBLE,
  open_interest BIGINT,
  close_price DOUBLE,
  settlement_price DOUBLE,
  upper_limit_price DOUBLE,
  lower_limit_price DOUBLE,
  pre_delta DOUBLE,
  curr_delta DOUBLE,
  level_1_bid_price DOUBLE,
  level_2_bid_price DOUBLE,
  level_3_bid_price DOUBLE,
  level_4_bid_price DOUBLE,
  level_5_bid_price DOUBLE,
  level_1_bid_volume BIGINT,
  level_2_bid_volume BIGINT,
  level_3_bid_volume BIGINT,
  level_4_bid_volume BIGINT,
  level_5_bid_volume BIGINT,
  level_1_ask_price DOUBLE,
  level_2_ask_price DOUBLE,
  level_3_ask_price DOUBLE,
  level_4_ask_price DOUBLE,
  level_5_ask_price DOUBLE,
  level_1_ask_volume BIGINT,
  level_2_ask_volume BIGINT,
  level_3_ask_volume BIGINT,
  level_4_ask_volume BIGINT,
  level_5_ask_volume BIGINT,
  average_price DOUBLE,
  action_day VARCHAR(50),
  trading_day VARCHAR(50),
  exchange_time VARCHAR(50),
  local_time VARCHAR(50)
);

INSERT INTO accounts VALUES
(
  'test_account',
  'cash: 0 id: "test_account" strategy_name: "test_strategy"'
) ON DUPLICATE KEY UPDATE account='cash: 0 id: "test_account" strategy_name: "test_strategy"';

CREATE TABLE IF NOT EXISTS securities (
	country VARCHAR(5),
	exchange VARCHAR(10),
	product VARCHAR(10),
	id VARCHAR(50),
	security VARCHAR(500),
	PRIMARY KEY (country, exchange, id)
);

CREATE TABLE IF NOT EXISTS trading_times (
	country VARCHAR(5),
	exchange VARCHAR(10),
	product VARCHAR(10),
	trading_time VARCHAR(100),
	PRIMARY KEY (country, exchange, product, trading_time)
);

INSERT INTO trading_times VALUES ('cn','CFFEX','IC',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CFFEX','IC',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CFFEX','IF',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CFFEX','IF',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CFFEX','IH',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CFFEX','IH',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CFFEX','T',' begin_time: "09:15:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CFFEX','T',' begin_time: "13:00:00" end_time: "15:15:00" ');
INSERT INTO trading_times VALUES ('cn','CFFEX','TF',' begin_time: "09:15:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CFFEX','TF',' begin_time: "13:00:00" end_time: "15:15:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','CF',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','CF',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','CF',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD CF&CF',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD CF&CF',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD CF&CF',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','FG',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','FG',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','FG',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD FG&FG',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD FG&FG',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD FG&FG',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','JR',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','JR',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD JR&JR',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD JR&JR',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','LR',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','LR',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD LR&LR',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD LR&LR',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','MA',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','MA',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','MA',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD MA&MA',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD MA&MA',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','OI',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','OI',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','OI',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD OI&OI',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD OI&OI',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD OI&OI',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','PM',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','PM',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD PM&PM',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD PM&PM',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','RI',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','RI',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD RI&RI',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD RI&RI',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','RM',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','RM',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','RM',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD RM&RM',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD RM&RM',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','RS',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','RS',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD RS&RS',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD RS&RS',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SF',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SF',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD SF&SF',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD SF&SF',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SM',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SM',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD SM&SM',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD SM&SM',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','IPS SF&SM',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','IPS SF&SM',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SR',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SR',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SR',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD SR&SR',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD SR&SR',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD SR&SR',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','TA',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','TA',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','TA',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD TA&TA',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD TA&TA',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD TA&TA',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','WH',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','WH',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD WH&WH',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD WH&WH',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','ZC',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','ZC',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','ZC',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD ZC&ZC',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD ZC&ZC',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','CZCE','SPD ZC&ZC',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','a',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','a',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','a',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SPC a&m',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SPC a&m',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SPC a&m',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP a&a',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP a&a',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP a&a',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','b',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','b',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','b',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP b&b',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP b&b',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP b&b',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','bb',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','bb',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','bb',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SPC fb&bb',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SPC fb&bb',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SPC fb&bb',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP bb&bb',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP bb&bb',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP bb&bb',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','c',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','c',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SPC c&cs',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SPC c&cs',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP c&c',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP c&c',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','cs',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','cs',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP cs&cs',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP cs&cs',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','fb',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','fb',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP fb&fb',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP fb&fb',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','i',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','i',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','i',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP i&i',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP i&i',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP i&i',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','j',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','j',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','j',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP j&j',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP j&j',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP j&j',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','jd',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','jd',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP jd&jd',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP jd&jd',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','jm',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','jm',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','jm',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP jm&jm',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP jm&jm',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP jm&jm',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','l',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','l',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SPC l&v',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SPC l&v',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP l&l',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP l&l',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','m',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','m',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','m',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP m&m',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP m&m',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP m&m',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','m_o',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','m_o',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','m_o',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','p',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','p',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','p',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP p&p',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP p&p',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP p&p',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','pp',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','pp',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP pp&pp',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP pp&pp',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','v',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','v',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP v&v',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP v&v',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','y',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','y',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','y',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SPC y&p',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SPC y&p',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SPC y&p',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP y&y',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP y&y',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','DCE','SP y&y',' begin_time: "21:00:00" end_time: "23:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','ag',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','ag',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','ag',' begin_time: "21:00:00" end_time: "02:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','agefp',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','agefp',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','agefp',' begin_time: "21:00:00" end_time: "02:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','al',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','al',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','al',' begin_time: "21:00:00" end_time: "01:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','alefp',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','alefp',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','alefp',' begin_time: "21:00:00" end_time: "01:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','au',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','au',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','au',' begin_time: "21:00:00" end_time: "02:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','auefp',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','auefp',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','auefp',' begin_time: "21:00:00" end_time: "02:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','bu',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','bu',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','bu',' begin_time: "21:00:00" end_time: "23:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','buefp',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','buefp',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','buefp',' begin_time: "21:00:00" end_time: "23:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','cu',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','cu',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','cu',' begin_time: "21:00:00" end_time: "01:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','cuefp',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','cuefp',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','cuefp',' begin_time: "21:00:00" end_time: "01:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','fu',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','fu',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','fuefp',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','fuefp',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','hc',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','hc',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','hc',' begin_time: "21:00:00" end_time: "23:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','hcefp',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','hcefp',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','hcefp',' begin_time: "21:00:00" end_time: "23:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','ni',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','ni',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','ni',' begin_time: "21:00:00" end_time: "01:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','niefp',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','niefp',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','niefp',' begin_time: "21:00:00" end_time: "01:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','pb',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','pb',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','pb',' begin_time: "21:00:00" end_time: "01:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','pbefp',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','pbefp',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','pbefp',' begin_time: "21:00:00" end_time: "01:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','rb',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','rb',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','rb',' begin_time: "21:00:00" end_time: "23:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','rbefp',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','rbefp',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','rbefp',' begin_time: "21:00:00" end_time: "23:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','ru',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','ru',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','ru',' begin_time: "21:00:00" end_time: "23:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','ruefp',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','ruefp',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','ruefp',' begin_time: "21:00:00" end_time: "23:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','sn',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','sn',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','sn',' begin_time: "21:00:00" end_time: "01:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','snefp',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','snefp',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','snefp',' begin_time: "21:00:00" end_time: "01:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','wr',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','wr',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','wrefp',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','wrefp',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','zn',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','zn',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','zn',' begin_time: "21:00:00" end_time: "01:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','znefp',' begin_time: "09:00:00" end_time: "11:30:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','znefp',' begin_time: "13:30:00" end_time: "15:00:00" ');
INSERT INTO trading_times VALUES ('cn','SHFE','znefp',' begin_time: "21:00:00" end_time: "01:00:00" ');