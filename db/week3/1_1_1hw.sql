\! echo 'Dropping the previous databases...'
drop TABLE if exists ORDERS;
drop TABLE if exists CUSTOMERS;
drop TABLE if exists PRODUCTS;
drop TABLE if exists AGENTS;

\! echo 'Create the table CUSTOMERS...'
CREATE TABLE CUSTOMERS
(
    cid INTEGER NOT NULL UNIQUE,
    cname CHAR(20),
    city CHAR(40),
    discnt FLOAT(8, 2),

    PRIMARY KEY (cid)
);

\! echo 'Create the table AGENTS...'
CREATE TABLE AGENTS
(
    aid INTEGER NOT NULL UNIQUE,
    aname CHAR(20),
    city CHAR(40),
    percent FLOAT(8, 2),

    PRIMARY KEY(aid)
);

\! echo 'Create the table PRODUCTS...'
CREATE TABLE PRODUCTS
(
    pid INTEGER NOT NULL UNIQUE,
    pname CHAR(20),

    PRIMARY KEY(pid)
);

\! echo 'Create the table ORDERS...'
CREATE TABLE ORDERS
(
    ordid INTEGER NOT NULL UNIQUE,
    cid INTEGER NOT NULL UNIQUE,
    aid INTEGER NOT NULL UNIQUE,
    pid INTEGER NOT NULL UNIQUE,

    month INTEGER,
    qty FLOAT(8, 2),
    dollars FLOAT(8, 2),

    FOREIGN KEY(cid)
        REFERENCES CUSTOMERS(cid)
        ON DELETE CASCADE,

    FOREIGN KEY(aid)
        REFERENCES AGENTS(aid)
        ON DELETE CASCADE,

    FOREIGN KEY(pid)
        REFERENCES PRODUCTS(pid)
        ON DELETE CASCADE,

    PRIMARY KEY(ordid)
);
