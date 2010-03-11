{Witamborski Szymon}

uses crt;

var
   koniec : boolean;
   c : char;
   znak : byte;
   jest : byte;
   f : text;

procedure zainicjuj;
begin
   asm
      mov ah,0
      mov al,11100011b {9600bd (111), zero parity control(00),
                      1 bit stopu(0), 8 bitow danych (11)}
      mov dx,00h  {uzyta bedzie funkjca 0, czyli inicjacja lacza}
      int 14h     {wywolanie przerwania 14h}

      {dalsza czesc sluzy do ustawienia prescallera zeby zwiekszyc predkosc do
      19200 bps}
                {com2 = 2f8h com1 = 3f8}
                {adres portu COM1 to 3f8h, COM2 to 2f8h, LCR dla nich to
                 adres portu +3}
                {AL to 8bitowy rejestr do naszego wlasnego uzytku, wiec
                 traktujmy to jako taka zmienna tymczasowa do wykonywania
                 obliczen, DX jest 16bitowy, chyba dlatego tam ladujemy adresy}
                {LCR = Line Control Register, ustawiamy tu parametry transmisji}
      mov dx,3fbh {zapisz do rejestru DX adres portu LCR}
      in al,dx    {pobierz zawartosc rejestru LCR portu COM1 do rejestru AL
                 cos jak w C: al = *dx}
      or al,10000000b {ustaw bit 7 na 1 (operacja OR zostawi reszte nietknieta)
                 w ten sposob dajemy znac, ze chcemy dobrac sie do dzielnika
                 predkosci, wtedy pierwsze 2 bajty rejestrow naszego portu
                 zamieniaja swoje znaczenie i zamiast bycia buforem odczytu
                 i zapisu mozemy tam wrzucic podzielnik predkosci;
                 cos jak w C: al |= 10000000b}
      out dx,al   {zapisz rejestr LCR z rejestru AL, cos jak: *dx = al}

      mov al,6    {wrzuc do rejestru AL 6, ktory zaraz ustawi nam dzielnik
                 predkosci, baza tutaj to 115200 czyli 115200/6 = 19200
                 czyli predkosc, ktora chcemy ustawic}
      mov dx,3f8h {wpisz do DX adres portu podzielnika (w oryginalnym komentarzu
                 bylo LSB czyli Least Signifant Byte, czyli ida tam 'mlodsze
                 bity', starsze trafia bajt dalej}
      out dx,al   {wpisz mlodsze bity podzielnika pod adres dx==3f8h}
      mov al,0    {teraz tutaj ustwiamy starsze bity}
      inc dx      {zwieksz adres ktory siedzial w dx, teraz dx==3f9h,
                 cos jak ++dx ;)}
      out dx,al   {wpisz starsze bity podzielnika pod nowy adres}

      {po tej operacji dwa pierwsze bajty (rejestry ukladu 16450) beda wygladaly tak:
      mlodsze  starsze bity
      00000110 00000000
      }

      {teraz jak juz ustawilismy podzielnik, zwracamy tym 2 pierwszym bajtom
      ich normalna funkce ustawiajac bit 7 w LCR na 0}
      mov dx,3fbh {znowu adres portu LCR}
      in al,dx    {pobierz zawartosc rejestru LCR portu COM1 do AL}
      and al,01111111b {wyzeruj bit 7, to jest iloczyn bitowy z przypisaniem,
                 czyli tam gdzie jedynki oryginalna wartosc sie nie zmieni}
      out dx,al   {zapisz rejestr LCR}
     end;       
end;

procedure wyslij;
begin
   asm
      mov ah,1
      mov al,znak
      int 14h
   end;
end;

procedure odbierz;
begin
   asm
      mov ah,2
      mov dx,0
      int 14h
      mov znak,al
   end;
end;

procedure status;
begin
   asm
      mov ah,3
      mov dx,0
      int 14h
      mov jest,ah
   end;
end;

begin
   clrscr;
   writeln('CTRL+S wysle program ZEGAREK.HEX');
   koniec := false;
   zainicjuj;
   repeat
      if KeyPressed then
      begin
         c := ReadKey;
         
         if ord(c) = 19 then
         begin
            {wysylanie programu}
            znak := ord('X');
            wyslij;
            znak := ord('3');
            wyslij;
            {otwarcie i wyslanie programu bajt po bajcie}
            assign(f, 'ZEGAREK.HEX');
            FileMode := 0;
            reset(f);
            while not eof(f) do
            begin
               repeat
                 status;
               until (jest and 32) = 32;
               read(f,c);
               znak := ord(c);
               wyslij;
            end;
         end

         else if ord(c) = 27 then
         begin
            koniec := true;
         end

         else
         begin
            {wysylanie bajtu z klawiatury}
            znak := ord(c);
            wyslij;
         end;
      end;
      
      status;
      if (jest and 1) = 1 then
      begin
         odbierz;
         write(chr(znak));
      end;
   until koniec;
end.