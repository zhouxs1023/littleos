{ generate the file random.c by Pascal Lacroix }

Uses Crt, Dos;

const
 header_len = 23;
 header : array[1..header_len] of string[80] = (
 '/*',
 ' *  LittleOS',
 ' *  Copyright ,(C) 1998 Lacroix Pascal (placr@mygale.org)',
 ' *',
 ' *  This program is free software; you can redistribute it and/or modify',
 ' *  it under the terms of the GNU General Public License as published by',
 ' *  the Free Software Foundation; either version 2 of the License, or',
 ' *  (at your option) any later version.',
 ' *',
 ' *  This program is distributed in the hope that it will be useful,',
 ' *  but WITHOUT ANY WARRANTY; without even the implied warranty of',
 ' *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the',
 ' *  GNU General Public License for more details.',
 ' *',
 ' *  You should have received a copy of the GNU General Public License',
 ' *  along with this program; if not, write to the Free Software',
 ' *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.',
 ' */',
 '',
 '/*',
 ' * random.c - generate random numbers',
 ' */',
 '');


const size = 2000;

var f: text;
    i, j: word;
    l: integer;
begin
     assign(f, 'random.c');
     rewrite(f);
     for i:= 1 to header_len do
       writeln(f, header[i]);

     randomize;
     writeln(f, '/* this file was generated do not modify (see gen_num.pas) */');
     writeln(f);
     writeln(f, '#define TABLE_SIZE ', size);
     writeln(f);
     writeln(f, 'static int rnd_table[TABLE_SIZE] = {');
     j:= 0;
     for i:= 1 to size do
       begin
         if j = 0 then write(f, '    ');
         l:= 0;
         repeat
           l:= random(256) * random(256);
         until l <> 0;

         write(f, l:6);
         if (j < 7) and (i < size) then
           begin
             write(f, ', ');
             inc(j);
           end
           else
         if (i = size) then
           begin
             writeln(f, ' };');
           end
           else
           begin
             writeln(f, ',');
             j:= 0;
           end;
       end;

     writeln(f);
     writeln(f, 'int seed = ', random(256) * random(256), ';'); writeln(f);
     writeln(f, 'extern unsigned long ticks;'); writeln(f);

     writeln(f, 'int srandom(int number)');
     writeln(f, '{ seed = number; return 0; }'); writeln(f);

     writeln(f, 'int rand()');
     writeln(f, '{');
     writeln(f, '    int i, j;');     writeln(f);
     writeln(f, '    i = rnd_table[seed % TABLE_SIZE];');
     writeln(f, '    j = rnd_table[i % TABLE_SIZE];');
     writeln(f, '    seed = i + j + (int)ticks;');
     writeln(f, '    return (seed * 2)/-3;');
     writeln(f, '}');
     close(f);
end.