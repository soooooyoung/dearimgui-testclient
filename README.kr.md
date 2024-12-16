# 테스트 클라이언트

이 프로젝트는 [Dear ImGui 라이브러리](https://github.com/ocornut/imgui)를 사용하여 개발된 테스트 클라이언트로, [이 저장소](https://github.com/soooooyoung/iocp-study)에 있는 서버를 테스트하기 위한 목적으로 제작되었습니다.

# 구현 과정

## Dear ImGUI 설정

> Dear ImGUI 공식 GitHub 저장소의 [examples 디렉토리](https://github.com/ocornut/imgui/tree/master/examples)를 참조하여 사용하는 플랫폼에 맞는 예제를 확인하세요.

Dear ImGUI의 다음 구성 파일을 포함합니다:

- **Win32 API**
- **DirectX11**
- **imgui_stdlib**
- **imgui.natstepfilter**
- **imgui.natvis**

Dear ImGUI는 크기가 작아 DLL로 분리하지 않고 파일을 직접 포함하여 사용하는 것이 권장됩니다.

### [DirectWindow](ImGuiSolution/DirectWindow.h)

`DirectWindow`는 Dear ImGui 렌더링 루프를 포함하여 애플리케이션의 메인 윈도우와 UI 렌더링을 관리합니다.

## 네트워크 작업 통합

<div style="display:flex">
<image src="images/brainfunction.jpg" width=200/>
<image src="images/advantage.jpg" width=200/>
</div>

대부분의 네트워크 작업은 테스트 서버의 재사용 가능한 코드를 기반으로 유사한 방식으로 동작합니다.

### [Network Manager](/ImGuiSolution/NetworkManager.h)

`NetworkManager` 클래스는 비동기 네트워크 작업 수행을 위하여 I/O Completion Port(IOCP) 객체를 생성 및 관리합니다.

### [클라이언트 연결](/ImGuiSolution/NetworkClient.h)

`NetworkClient` 인스턴스는 Connect 명령에 의해 추가되며, 각 클라이언트는 네트워크 작업을 개별적으로 처리합니다. 서버와의 연결 을 비동기적으로 처리하기 위해 `ConnectEx`를 사용합니다.

## 명령 처리

`DirectWindow`는 UI 동작(예: 버튼 클릭)을 네트워크 작업에 연결하는 콜백을 통해 명령을 호출합니다.

구조화된 명령 [Command](/ImGuiSolution/Command.h)는 [App](ImGuiSolution/App.h)의 `MainLoop()`에서 순차적으로 큐에 추가되어 처리됩니다. 이는 다음 렌더링 사이클 이전에 모든 명령이 처리되도록 하여 레이스 컨디션을 방지합니다.
