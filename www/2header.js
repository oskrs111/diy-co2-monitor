const fs = require("fs");
const source = "./www/index.html";
const target = "./www/index.h";
let raw = fs.readFileSync(source).toString();
let clean = raw.replace(/\r\n/g, "");
let write = "";
write += `#ifndef INDEX_H\r\n`;
write += `#define INDEX_H\r\n`;

write += `#include <stdint.h>\r\n`;
write += `#ifdef ARDUINO\r\n`;
write += `#define PROGMEM\r\n`;
write += `#endif\r\n`;

write += `#define html_length ${clean.length + 1}\r\n`;
write += `const char html_buffer[] PROGMEM = {\r\n`;
let crln = 0;
for (let t = 0; t < clean.length; t++) {
  write += clean.charCodeAt(t) + ",";
  if (crln == 25) {
    write += "\r\n";
    crln = 0;
  } else {
    crln++;
  }
}
write += `0};\r\n`;
write += `#endif\r\n`;
write += `/**< Conversion completed at ${new Date().getTime()} */ \r\n`;

fs.writeFileSync(target, write);
console.log(">>> created 'index.h' with lenght= " + write.length);
