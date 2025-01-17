import sys, mmap, re
import sqlite3
conn = None             # sqlite3 db connection
c = None                # sqlite3 db cursor

curr = dict()
temp = list()
d_prefix = {}

def dbInsert():
    c.execute("INSERT INTO rdf VALUES ('{}','{}','{}')".format(curr['sub'], curr['pred'], curr['obj'])


def parsePrefix(dataLine):
    dataLine = dataLine.replace(' ', '\t');
    tag, pref, iri, term= dataLine.split('\t');
    # print(tag, pref, iri, term);
    if tag =='@prefix' :
        # check the prefix
        if pref[-1] != ':':
            print(">> Missing colon")
            return False        # Error! Missing Colon

        # parse the iri
        if term.strip('\n') == '.':
            # print("print to file")
            # f_prefix.write(pref + '\t' +iri+ '\n')
            d_prefix[pref] = iri.strip('<>')
            print d_prefix


def parse_rdf():
   flag = 'null'
   with open ("Edmonton.txt", "r", encoding = 'utf8') as a:
      with open ("parsed_results.txt", "w", encoding = 'utf8') as b:
         for lin in a:
            if "@" in lin and ('@en' not in lin):
               #print(lin)
               continue

            if "@" in lin and ('@prefix' in lin):
                parsePrefix(lin);
                continue
            # print(lin)
            # temp = lin.replace("\t", '\n').split()
            temp = lin.replace('\n','').replace('@en','').split("\t")
            # print(temp)
            # print("flag is ", flag)


            if len(temp) != 3:
               break;


            if (temp[0] and temp[1] and temp[2]):
               print('case 1')
               flag = temp[2][-1]
               curr['sub']=temp[0]
               curr['pred']=temp[1]
               curr['obj']=temp[2][:-2]
               check(curr['sub'], curr['pred'], curr['obj'])
               b.write(curr['sub']+"\t"+curr['pred']+"\t"+curr['obj']+"\n")
            elif (flag == ';'):
               print('case 2')
               flag = temp[2][-1]
               curr['pred'] = temp[1]
               curr['obj'] = temp[2][:-2]
               check(curr['sub'], curr['pred'], curr['obj'])
               b.write(curr['sub']+"\t"+curr['pred']+"\t"+curr['obj']+"\n")
            elif (flag == ','):
               print('case 3')
               flag = temp[2][-1]
               curr['obj'] = temp[2][:-2]
               check(curr['sub'], curr['pred'], curr['obj'])
               b.write(curr['sub']+"\t"+curr['pred']+"\t"+curr['obj']+"\n")
            else:
               print("no match", temp)








   a.close()
   b.close()

def check(sub, pred, obj):
   pass


###### Main ###########################
if __name__ == "__main__":
    argv = sys.argv

    if len(argv) == 3:
        sqldb = argv[1]
        filename = argv[2]
        conn = sqlite3.connect(sqldb)
        c = conn.cursor()
        parse_rdf(filename)
        conn.close()

    else:
        print("Usage: "+ argv[0] + " <sql-database> <rdf>\n")




##### END #############################
