var Anyterm = function() {
  this.init();
}

Anyterm.prototype = {
  init: function() {
    $.ajaxSetup({
      ifModified: true,
      cache: false
    });
  },

  terminalContents: '',
  initialTimeout: 50,
  timeoutFactor: 2,
  timeoutId: 0,
  timeout: 50,
  writeBuffer: '',
  writing: false,

  resetTimeout: function() {
    Anyterm.timeout = Anyterm.initialTimeout;
  },
  increaseTimeout: function() {
    Anyterm.timeout *= Anyterm.timeoutFactor;
  },

  getRowCount: function() {
    return Math.round($('#terminal').innerHeight() / $('#metric').innerHeight());
  },
  getColumnCount: function() {
    return Math.round($('#terminal').innerWidth() / $('#metric').innerWidth());
  },

  onOpened: function(data) {
    Anyterm.read();
  },

  open: function(username) {
    $.post('anytermd', { action: 'open', row_count: Anyterm.getRowCount(), column_count: Anyterm.getColumnCount(), username: username, dummy: '' }, Anyterm.onOpened);
  },

  resize: function() {
    $.post('anytermd', { action: 'resize', row_count: Anyterm.getRowCount(), column_count: Anyterm.getColumnCount(), dummy: '' });
  },

  onRead: function(text, status, request) {
    if(status == "notmodified") {
      $('#terminal').html(Anyterm.terminalContents);
      Anyterm.increaseTimeout();
    } else if(status == "success") {
      Anyterm.terminalContents = text;
      Anyterm.resetTimeout();
    } else if(status == "error") {
      $('#terminal').html(text);
      Anyterm.increaseTimeout();
    } 

    if(Anyterm.timeoutId == 0) {
      Anyterm.timeoutId = window.setTimeout(Anyterm.read, Anyterm.timeout);
    }
  },

  read: function() {
    if(Anyterm.timeoutId > 0) {
      window.clearTimeout(Anyterm.timeoutId);
      Anyterm.timeoutId = 0;
    }

    $('#terminal').load('anytermd?action=read&dummy=', Anyterm.onRead);
  },

  onWrite: function(data) {
    if(Anyterm.timeoutId > 0) {
      window.clearTimeout(Anyterm.timeoutId);
      Anyterm.timeoutId = 0;
    }

    Anyterm.timeoutId = window.setTimeout(Anyterm.read, Anyterm.initialTimeout);

    if(Anyterm.writeBuffer == '') {
      Anyterm.writing = false;
    } else {
      Anyterm.flushWriteBuffer();
    }
  },

  flushWriteBuffer: function() {
    var value = Anyterm.writeBuffer;
    Anyterm.writeBuffer = '';
    Anyterm.writing = true;
    $.post('anytermd', { action: 'write', data: value, dummy: '' }, Anyterm.onWrite);
  },

  write: function(value) {
    Anyterm.writeBuffer += value;
    if(!Anyterm.writing) {
      Anyterm.flushWriteBuffer();
    }
  }
};

$(document).ready(function() {
  Anyterm = new Anyterm();
});

$(window).resize(function() {
  Anyterm.resize();
});

$(document).keypress(function(event) {
  if(!(event.ctrlKey || event.altKey)) {
    Anyterm.write(String.fromCharCode(event.which));
  }
});

var ESC = String.fromCharCode(0x1B);

var codes = {
// C0 codes
  NUL: String.fromCharCode(0x00),
  SOH: String.fromCharCode(0x01),
  STX: String.fromCharCode(0x02),
  ETX: String.fromCharCode(0x03),
  EOT: String.fromCharCode(0x04),
  ENQ: String.fromCharCode(0x05),
  ACK: String.fromCharCode(0x06),
  BEL: String.fromCharCode(0x07),
  BS : String.fromCharCode(0x08),
  HT : String.fromCharCode(0x09),
  LF : String.fromCharCode(0x0A),
  VT : String.fromCharCode(0x0B),
  FF : String.fromCharCode(0x0C),
  CR : String.fromCharCode(0x0D),
  SO : String.fromCharCode(0x0E),
  SI : String.fromCharCode(0x0F),
  DLE: String.fromCharCode(0x10),
  DC1: String.fromCharCode(0x11),
  DC2: String.fromCharCode(0x12),
  DC3: String.fromCharCode(0x13),
  DC4: String.fromCharCode(0x14),
  NAK: String.fromCharCode(0x15),
  SYN: String.fromCharCode(0x16),
  ETB: String.fromCharCode(0x17),
  CAN: String.fromCharCode(0x18),
  EM : String.fromCharCode(0x19),
  SUB: String.fromCharCode(0x1A),
  ESC: String.fromCharCode(0x1B),
  FS : String.fromCharCode(0x1C),
  GS : String.fromCharCode(0x1D),
  RS : String.fromCharCode(0x1E),
  US : String.fromCharCode(0x1F),
  DEL: String.fromCharCode(0x7F),

// C1 codes
  IND: ESC + 'D', // String.fromCharCode(0x84),
  NEL: ESC + 'E', // String.fromCharCode(0x85),
  HTS: ESC + 'H', // String.fromCharCode(0x88),
  RI : ESC + 'M', // String.fromCharCode(0x8D),
  SS2: ESC + 'N', // String.fromCharCode(0x8E),
  SS3: ESC + 'O', // String.fromCharCode(0x8F),
  DCS: ESC + 'P', // String.fromCharCode(0x90),
  SPA: ESC + 'V', // String.fromCharCode(0x96),
  EPA: ESC + 'W', // String.fromCharCode(0x97),
  SOS: ESC + 'X', // String.fromCharCode(0x98),
  DEC: ESC + 'Z', // String.fromCharCode(0x9A),
  CSI: ESC + '[', // String.fromCharCode(0x9B),
  ST : ESC + '\\', // String.fromCharCode(0x9C),
  OSC: ESC + ']', // String.fromCharCode(0x9D),
  PM : ESC + '^', // String.fromCharCode(0x9E),
  APC: ESC + '_', // String.fromCharCode(0x9F),
};

keyMapping = {
  '45':  { code: codes.CSI, number: '2',  character: '~' }, // Insert
  '46':  { code: codes.CSI, number: '3',  character: '~' }, // Suppr
  '33':  { code: codes.CSI, number: '5',  character: '~' }, // Page Up
  '34':  { code: codes.CSI, number: '6',  character: '~' }, // Page Down

  '35':  { code: codes.SS3, number:  '',  character: 'F' }, // End
  '36':  { code: codes.SS3, number:  '',  character: 'H' }, // Home

  '37':  { code: codes.SS3, number:  '',  character: 'D', modCode: codes.CSI }, // Left
  '38':  { code: codes.SS3, number:  '',  character: 'A', modCode: codes.CSI }, // Up
  '39':  { code: codes.SS3, number:  '',  character: 'C', modCode: codes.CSI }, // Right
  '40':  { code: codes.SS3, number:  '',  character: 'B', modCode: codes.CSI }, // Down

  '112': { code: codes.SS3, number:  '',  character: 'P' }, // F1
  '113': { code: codes.SS3, number:  '',  character: 'Q' }, // F2
  '114': { code: codes.SS3, number:  '',  character: 'R' }, // F3
  '115': { code: codes.SS3, number:  '',  character: 'S' }, // F4

  '116': { code: codes.CSI, number: '15', character: '~' }, // F5

  '117': { code: codes.CSI, number: '17', character: '~' }, // F6
  '118': { code: codes.CSI, number: '18', character: '~' }, // F7
  '119': { code: codes.CSI, number: '19', character: '~' }, // F8
  '120': { code: codes.CSI, number: '20', character: '~' }, // F9
  '121': { code: codes.CSI, number: '21', character: '~' }, // F10
  '122': { code: codes.CSI, number: '23', character: '~' }, // F11
  '123': { code: codes.CSI, number: '24', character: '~' }, // F12
}

getModified = function(event, termCode) {
  if(termCode.modifiable != undefined && !termCode.modifiable) {
    return termCode.number + termCode.character;
  }

  number = (termCode.number == undefined || termCode.number == '') ? '1' : termCode.number;
  code = (termCode.modCode == undefined) ? termCode.code : termCode.modCode;
  character = (termCode.modChar == undefined) ? termCode.character : termCode.modChar;

  if(!event.ctrlKey && !event.shiftKey && !event.altKey) {
    return termCode.code + termCode.number + termCode.character;
  } else if(!event.ctrlKey &&  event.shiftKey && !event.altKey) {
    return code + number + ';2' + character;
  } else if(!event.ctrlKey && !event.shiftKey &&  event.altKey) {
    return code + number + ';3' + character;
  } else if(!event.ctrlKey &&  event.shiftKey &&  event.altKey) {
    return code + number + ';4' + character;
  } else if( event.ctrlKey && !event.shiftKey && !event.altKey) {
    return code + number + ';5' + character;
  } else if( event.ctrlKey &&  event.shiftKey && !event.altKey) {
    return code + number + ';6' + character;
  } else if( event.ctrlKey && !event.shiftKey &&  event.altKey) {
    return code + number + ';7' + character;
  } else if( event.ctrlKey &&  event.shiftKey &&  event.altKey) {
    return code + number + ';8' + character;
  }
}

$(document).keydown(function(event) {
  switch(event.which) {
    case 16:
    case 17:
    case 18:
      return true;
  }

  if(keyMapping[event.which] != undefined) {
    Anyterm.write(getModified(event, keyMapping[event.which]));
    return false;
  }

  if(event.ctrlKey) {
    switch(event.which) {
      case 50:
      case '@':
        Anyterm.write(codes.NUL);
        return false;
      case 65:
      case 'A':
        Anyterm.write(codes.SOH);
        return false;
      case 66:
      case 'B':
        Anyterm.write(codes.STX);
        return false;
      case 67:
      case 'C':
        Anyterm.write(codes.ETX);
        return false;
      case 68:
      case 'D':
        Anyterm.write(codes.EOT);
        return false;
      case 69:
      case 'E':
        Anyterm.write(codes.ENQ);
        return false;
      case 70:
      case 'F':
        Anyterm.write(codes.ACK);
        return false;
      case 71:
      case 'G':
        Anyterm.write(codes.BEL);
        return false;
      case 72:
      case 'H':
        Anyterm.write(codes.BS);
        return false;
      case 73:
      case 'I':
        Anyterm.write(codes.HT);
        return false;
      case 74:
      case 'J':
        Anyterm.write(codes.LF);
        return false;
      case 75:
      case 'K':
        Anyterm.write(codes.VT);
        return false;
      case 76:
      case 'L':
        Anyterm.write(codes.FF);
        return false;
      case 77:
      case 'M':
        Anyterm.write(codes.CR);
        return false;
      case 78:
      case 'N':
        Anyterm.write(codes.SO);
        return false;
      case 79:
      case 'O':
        Anyterm.write(codes.SI);
        return false;
      case 80:
      case 'P':
        Anyterm.write(codes.DLE);
        return false;
      case 81:
      case 'Q':
        Anyterm.write(codes.DC1);
        return false;
      case 82:
      case 'R':
        Anyterm.write(codes.DC2);
        return false;
      case 83:
      case 'S':
        Anyterm.write(codes.DC3);
        return false;
      case 84:
      case 'T':
        Anyterm.write(codes.DC4);
        return false;
      case 85:
      case 'U':
        Anyterm.write(codes.NAK);
        return false;
      case 86:
      case 'V':
        Anyterm.write(codes.SYN);
        return false;
      case 87:
      case 'W':
        Anyterm.write(codes.ETB);
        return false;
      case 88:
      case 'X':
        Anyterm.write(codes.CAN);
        return false;
      case 89:
      case 'Y':
        Anyterm.write(codes.EM);
        return false;
      case 90:
      case 'Z':
        Anyterm.write(codes.SUB);
        return false;
      case 219:
      case '[':
        Anyterm.write(codes.ESC);
        return false;
      case 220:
      case '\\':
        Anyterm.write(codes.FS);
        return false;
      case 221:
      case ']':
        Anyterm.write(codes.GS);
        return false;
      case 222:
      case '^':
        Anyterm.write(codes.RS);
        return false;
      case 189:
      case '_':
        Anyterm.write(codes.US);
        return false;
      case 188:
      case '?':
        Anyterm.write(codes.DEL);
        return false;
    }
  } else if(event.shiftKey) {
    switch(event.which) {
      case 9:
        Anyterm.write(codes.ESC + '[Z');
        return false;
    }
  } else {
    switch(event.which) {
      case 8:
        Anyterm.write(codes.BS);
        return false;
      case 9:
        Anyterm.write(codes.HT);
        return false;
    }
  }

  switch(event.which) {
    case 65:
    case 66:
    case 67:
    case 68:
    case 69:
    case 70:
    case 71:
    case 72:
    case 73:
    case 74:
    case 75:
    case 76:
    case 77:
    case 78:
    case 79:
    case 80:
    case 81:
    case 82:
    case 83:
    case 84:
    case 85:
    case 86:
    case 87:
    case 88:
    case 89:
    case 90:
    case 96:
    case 97:
    case 98:
    case 99:
    case 100:
    case 101:
    case 102:
    case 103:
    case 104:
    case 105:
      return false;
  }

  return false;
});

