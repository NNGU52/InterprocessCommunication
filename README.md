# InterprocessCommunication
Межпроцессорное взаимодействие.

## Постановка задачи
Синхронизация потоков. Два экземпляра одного приложения работают в связке: один экземпляр выступает в качестве отправителя данных, другой - в качестве получателя. Для настройки на нужный режим работы и для сигнализации о наличии данных для приёма процессы одновременно используют несколько именованных объектов-событий. Первый экземпляр создаёт объект-событие с ручным сбросом, сигнальное состояние которого указывает, что отправитель существует. Второй (и последующие) экземпляры определяют это (функцией WaitForSingleObject) и настраиваются на режим получателя. О наличии новой порции данных для приёма отправитель сообщает получателю при помощи другого объекта-события. **Примечание:** если возможно, использовать один из механизмов межпроцессорного взаимодействия для передачи данных. В противном случае, просто отобразить *факт* получения сигнала о наличии данных.

## Интерфейс программы
<p align="center"><img src="/screenshots/1.png"/></p>
<p align="center"><img src="/screenshots/2.png"/></p>
<p align="center"><img src="/screenshots/3.png"/></p>

### P.S.
Visual Studio 2013, MFC
